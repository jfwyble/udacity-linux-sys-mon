#include <string>
#include <iomanip>
#include <sstream>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) 
{
    long hours = seconds / 3600;
    // To get the minutes, get the modulus of 
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    std::stringstream s;
    
    s << std::setfill('0') << std::setw(2) << hours << ':' << std::setfill('0') << std::setw(2) << minutes << ':' << std::setfill('0') << std::setw(2) << secs;
     return s.str();  
}