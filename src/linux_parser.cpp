#include <dirent.h>
#include <unistd.h>
#include <assert.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::cout;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{
  string line;

  string cat, units;
  int val;
  const static string memTotal = "MemTotal:",
    memFree = "MemFree:",
    kb = "kB";

  float memTotalVal, memFreeVal;
  float memUsagePercent;
  bool success = false;
  //cout << "Opening file: " << kProcDirectory + kMeminfoFilename << std::endl;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(stream)
    {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> cat >> val >> units;
        if(cat == memTotal && units == kb)
        {
          //cout<< val << std::endl;
          memTotalVal = val;
        }
        else if(cat == memFree && units == kb)
        {
          //cout<< val << std::endl;
          memFreeVal = val;
          memUsagePercent = memFreeVal / memTotalVal;
          //cout << memUsagePercent << std::endl;
          success = true;
          break;
        }
    }
  }
  stream.close();
  if(!success)
    throw std::runtime_error(
      string(__func__)
      .append(": Failed to parse values for system memory usage."));
  //printf("Mem usage: %.2f\n", memUsagePercent);    
  return memUsagePercent;
}

// Read and return the system uptime
long LinuxParser::UpTime() 
{
  string line;

  double valUptime, valIdle;
  long uptime;
  // bool success = false;
  //cout << "Opening file: " << kProcDirectory + kMeminfoFilename << std::endl;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> valUptime >> valIdle;
       if(!linestream.fail())
       {
         uptime = valUptime;
       }
       else
       {
            throw std::runtime_error(
              string(__func__)
              .append(": Failed to parse values for system uptime."));

       }
  }
  stream.close();

  return uptime;
}


// Read and return the number of jiffies for the system
// Confusing for students. We're on kenrel 4.15 and
// jiffies ended with kernel 2.6
// I'm interpreting this as a factor query to get clock ticks per second.
long LinuxParser::Jiffies() 
{
  static const long clockTicksPerSecond = sysconf(_SC_CLK_TCK);
  return clockTicksPerSecond;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) 
{

  // TODO - open the /proc/[PID]/stat file and get a sum of
  // item numbers 14-17
// const std::string kProcDirectory{"/proc/"};
// const std::string kCmdlineFilename{"/cmdline"};
// const std::string kCpuinfoFilename{"/cpuinfo"};
// const std::string kStatusFilename{"/status"};
// const std::string kStatFilename{"/stat"};
// const std::string kUptimeFilename{"/uptime"};
// const std::string kMeminfoFilename{"/meminfo"};
// const std::string kVersionFilename{"/version"};
// const std::string kOSPath{"/etc/os-release"};
// const std::string kPasswordPath{"/etc/passwd"};

  bool success = false;

  string line, val;
  long utime, stime, cuttime, cstime, starttime;

  string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  std::ifstream filestream(path);
  if(filestream.is_open())
  {
      // There are a huge number of values in one line:
      //
      if(std::getline(filestream, line))
      {
        std::istringstream input(line);

        for(int i = 0; i < 22; i++)
        {
            input >> val;
            if(!input) break;
            else
            {
              switch(i)
              {
                case 13:
                  utime = stol(val);
                  break;
                case 14:
                  stime = stol(val);
                  break;
                case 15:
                  cuttime = stol(val);
                  break;
                case 16:
                  cstime = stol(val);
                  break;
                case 21:
                  starttime = stol(val);
                  success = true;
                  break;
              }
            }
        }
      }
  }

  filestream.close();

  if(!success)
    throw std::runtime_error(
      std::string(__func__)
      .append(": Failed to parse process usage stats for PID ")
      .append(std::to_string(pid))
    );

  return utime + stime + cuttime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<int> LinuxParser::CpuUtilization() 
{
  vector<int> cpuStats;
  
  string line;

  string cat;
  int val;

  int totalProcesses;
  const static string cpu = "cpu";
  
  bool success = false;
 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(stream)
    {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> cat;
        
        if(linestream && cat == cpu)
        {
             for(int i = 0; i < 10; i++) 
             {
               linestream >> val;
               cpuStats.push_back(val);
             } 
             success = true;
        }
    }
  }
  stream.close();
  if(!success)
    throw std::runtime_error(
      string(__func__)
      .append(": Failed to parse values for aggregate cpu stats."));
  //printf("Mem usage: %.2f\n", memUsagePercent);    
 
  return cpuStats;

}

// Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  string line;

  string cat;
  int val;

  int totalProcesses;
  const static string cpu = "processes";
  
  bool success = false;
 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(stream)
    {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> cat >> val;
        if(linestream && cat == cpu)
        {
            totalProcesses = val;
            success = true;
        }
    }
  }
  stream.close();
  if(!success)
    throw std::runtime_error(
      string(__func__)
      .append(": Failed to parse values for total process count."));
  //printf("Mem usage: %.2f\n", memUsagePercent);    
 
  return totalProcesses;
}

static void dumpString(const std::string &s)
{
   cout << std::hex;
    for(size_t i = 0u; i <s.length(); i++)
    {
      cout << (int)s[i] << ", ";
    }
    cout << std::endl;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{

  string line;

  string hdr1;
  int val;

  int runningProcs = -1;
  const static string procs = "procs_running";
  
  bool success = false;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(stream)
    {
        if(std::getline(stream, line))
        {

       
        std::istringstream linestream(line);
        // cout << "Line read: '" << line << '\'' << std::endl;
        linestream >> hdr1 >> val;
        
       // cout << "hdr1 token: '" << hdr1 << "'\tval: " << val << std::endl;
        // cout << hdr1.length() << '\t' << procs.length() << std::endl;
        // cout << ((hdr1 == procs) ? "true" : "false") << std::endl;
        // dumpString(hdr1);
        // dumpString(procs);
        if(linestream && hdr1 == procs)
        {
            //cout << val << std::endl;
            runningProcs = val;
            success = true;
            break;
        }
        }
    }
  }
  stream.close();
  if(!success)
  {
    throw std::runtime_error(
      string(__func__)
      .append(": Failed to parse values for running processes count."));    
  }
  return runningProcs;

}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{
  bool success = false;
  string commandLine;
        std::string path = LinuxParser::kProcDirectory +
            std::to_string(pid) + LinuxParser::kCmdlineFilename;
        
   // std::cout << path << std::endl;
    std::ifstream filestream(path);
    
    string line;
    
    if(filestream.is_open())
    {
        if(std::getline(filestream, line))
        {
            commandLine = line;
            success = true;
            
        }
    }
    filestream.close();
    if (!success)
    throw std::runtime_error(
        std::string(__func__)
            .append("Unable to parse command line information for process with PID ")
            .append(std::to_string(pid)));

   return commandLine; 
}

// Read and return the memory used by a process
int LinuxParser::Ram(int pid) 
{

  string line;

  string ram;
  int val;
  string hdr, units;

  // int runningProcs = -1;
  const static string uidHdr = "VmSize:",
    kb = "kB";
  
  bool success = false;
  
  string path = kProcDirectory + std::to_string(pid) + 
    kStatusFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    while(stream)
    {
        if(std::getline(stream, line))
        {

       
        std::istringstream linestream(line);
        // cout << "Line read: '" << line << '\'' << std::endl;
        linestream >> hdr >> val >> units;
  
        if(linestream && hdr == uidHdr && units == kb)
        {
            //cout << val << std::endl;
            ram = val;
            success = true;
            break;
        }
        }
    }
  }
  stream.close();
  if(!success)
  {
    throw std::runtime_error(
      string(__func__)
      .append(": Failed to parse UID for PID ")
      .append(std::to_string(pid)));    
  }
 // cout << __func__ << "Returning UID :" << uid << std::endl;
  // std::stringstream s;
  // s << val << " " << units;
  // ram = s.str();
  return val;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{

  string line;

  string uid;
  string hdr, val;

  // int runningProcs = -1;
  const static string uidHdr = "Uid:";
  
  bool success = false;
  
  string path = kProcDirectory + std::to_string(pid) + 
    kStatusFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    while(stream)
    {
        if(std::getline(stream, line))
        {

       
        std::istringstream linestream(line);
        // cout << "Line read: '" << line << '\'' << std::endl;
        linestream >> hdr >> val;
        

      /*
        I had issues from the combination of the istringstream tokenizing the two strings
        with an underscore, and something apparently going on with the std out of the process,
        causing the underscore to display as a spave in cout. So I had to check the ascii values
        of every char.
      */

       // cout << "hdr1 token: '" << hdr1 << "'\tval: " << val << std::endl;
        // cout << hdr1.length() << '\t' << procs.length() << std::endl;
        // cout << ((hdr1 == procs) ? "true" : "false") << std::endl;
        // dumpString(hdr1);
        // dumpString(procs);
        if(linestream && hdr == uidHdr)
        {
            //cout << val << std::endl;
            uid = val;
            success = true;
            break;
        }
        }
    }
  }
  stream.close();
  if(!success)
  {
    throw std::runtime_error(
      string(__func__)
      .append(": Failed to parse UID for PID ")
      .append(std::to_string(pid)));    
  }
  // cout << __func__ << "Returning UID :" << uid << std::endl;
  return uid;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) 
{
  static std::map<int, string> usersById;

  if(usersById.empty())
  {
      string line;
      std::ifstream stream(kPasswordPath);
      if (stream.is_open()) {
      while(stream)
      {
          if(std::getline(stream, line))
          {

            int userId;
            string passwordHash, userName; 
            string token;
            int index = 0;

            std::istringstream linestream(line);
            while(std::getline(linestream, token, ':'))
            {
                switch(index)
                {
                  case 0:
                    userName = token;
                    break;
                  case 1:
                    passwordHash = token;
                    break;
                  case 2:
                    userId = stoi(token);
                    usersById[userId] = userName;
                    break;

                }
                if(++index == 3) break;
            }
        }
    }
  }    
  }

  // Obtain the UID for the PID, *then* use the database to get the name
  string uidStr = Uid(pid);
  int uid = stoi(uidStr);

  auto found = usersById.find(uid), end = usersById.end();
  if(found != end)
  {
    return(*found).second;
  }

  throw std::runtime_error(
    std::string(__func__)
      .append(": Unable to locate user name entry for UID ")
      .append(uidStr)
  );
}

// Read and return the uptime of a process
// I'm using this to get the start time, in clock ticks
long LinuxParser::UpTime(int pid) 
{
  // Open /proc/[PID]/stat and get the start time of the process (token #22)
  // Compare this with now, and you'll have the age of the process:
  //

  bool success = false;

  string line, val;
  long starttime;

  string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  std::ifstream filestream(path);
  if(filestream.is_open())
  {
      // There are a huge number of values in one line:
      //
      if(std::getline(filestream, line))
      {
        std::istringstream input(line);

        for(int i = 0; i < 22; i++)
        {
            input >> val;
            if(!input) break;
            else if(i == 21)
            {
                  starttime = stol(val);
                  success = true;
            }
        }
      }
  }

  filestream.close();

  if(!success)
    throw std::runtime_error(
      std::string(__func__)
      .append(": Failed to parse process start time for PID ")
      .append(std::to_string(pid))
    );

  return starttime;
}