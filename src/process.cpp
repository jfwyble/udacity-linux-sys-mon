#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <assert.h>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) :
    pid_(pid)
{

}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() 
{
    // TODO (for me) - bonus "real time" utilization by caching previous readings

    /*
    I had problems doing that with the one second interval causing a zero seconds uptime diff
    between readings.

    Since processes can be terminated, we have to keep getting the list from the kernel.

    So, to allow for persistent process objects in the vector, we'd have to make this program
    more sophisticated, such as with read time caching or multiple threads.
    */

    double secondsUptime = this->UpTime();
    double ticksActive = LinuxParser::ActiveJiffies(pid_);

    return cpuUtilization_ = (ticksActive / (secondsUptime * LinuxParser::Jiffies()));
}

// Return the command that generated this process
string Process::Command() 
{  
    if(commandLine_.empty())
    {
       commandLine_ = LinuxParser::Command(pid_);
    }

    return commandLine_; 
}

// Return this process's memory utilization
string Process::Ram() 
{
    int ramKb = LinuxParser::Ram(pid_);
    float ramMb = (float)ramKb / 1000.f;
    std::stringstream s;

    s << std::fixed << std::setprecision(1) << ramMb;
    return s.str();
}

// Return the user (name) that generated this process
string Process::User() 
{   
    if(user_.empty())
    {
       user_ = LinuxParser::User(pid_);
    }
    return user_;
}

// Return the age of this process (in seconds)
long int Process::UpTime() 
{
    long sysUptimeInSeconds = LinuxParser::UpTime();
    // This function in LinuxParser oughtta be called 'StartTime(int)'
    long clockTicksAtProcStart = LinuxParser::UpTime(pid_);

    long processUpTimeInseconds =
        sysUptimeInSeconds - (clockTicksAtProcStart/LinuxParser::Jiffies());
    return processUpTimeInseconds;
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const
{
    return this->cpuUtilization_ > a.cpuUtilization_;
}