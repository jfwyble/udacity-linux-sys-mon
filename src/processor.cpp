#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() 
{
    

    auto procStats = LinuxParser::CpuUtilization();

     user = procStats[0] - previous[0];
     nice = procStats[1] - previous[1];
     system = procStats[2] - previous[2];
     idle =   procStats[3] - previous[3];
     iowait = procStats[4] - previous[4];
     irq = procStats[5] - previous[5];
     softirq = procStats[6] - previous[6];
     steal = procStats[7] - previous[7];
     guest = procStats[8] - previous[8];
     guest_nice = procStats[9] - previous[9];
    
    // These are only total on the first reading.
    // Now, they are relatived since last measurement
    int total_cpu_time_since_boot_or_last
     = user + nice + system + idle + iowait + irq + softirq + steal;
    int idle_cpu_time_since_boot_or_last = idle + iowait;
    int active_cpu_time_since_boot_or_last = total_cpu_time_since_boot_or_last
     - idle_cpu_time_since_boot_or_last;

    // set previous to current, to be used in the next reading:
    //
    std::copy(procStats.begin(), procStats.end(), previous.begin());

    return (float)active_cpu_time_since_boot_or_last / (float)total_cpu_time_since_boot_or_last;
    ;
}