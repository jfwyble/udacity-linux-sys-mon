#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() 
{
    /*
    I'm unsure of the benefit of caching the process data
    is here, since the calling code pretty much calls this
    at regular intervals from a single thread.
    Unless we checked timestamps and only hit the system again
    if the readings happen very frequently.
    */
    processes_.clear();
    for(const int & pid : LinuxParser::Pids())
    {
        processes_.push_back(Process(pid));
    }
    
    for(Process & p : processes_)
        p.CpuUtilization();

    std::sort(processes_.begin(), processes_.end());

    return processes_; 
}

// Return the system's kernel identifier (string)
std::string System::Kernel() 
{
    return LinuxParser::Kernel();
}

// Return the system's memory utilization
float System::MemoryUtilization() 
{
    return LinuxParser::MemoryUtilization();
}

// Return the operating system name
std::string System::OperatingSystem()
{
    return LinuxParser::OperatingSystem(); 
}

// Return the number of processes actively running on the system
int System::RunningProcesses() 
{
    return LinuxParser::RunningProcesses();
}

// Return the total number of processes on the system
int System::TotalProcesses() 
{
    return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long int System::UpTime() 
{
    return LinuxParser::UpTime();
}