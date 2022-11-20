#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) 
{ 
    // Compute the date
    int minute = (seconds/60)%60;
    int hour = seconds/3600;
    int lSeconds = seconds%60;

    // Find the correct prefix
    string preHour = (hour < 10) ? "0" : "";
    string preMinute = (minute < 10) ? "0" : "";
    string preSeconds = (lSeconds < 10) ? "0" : "";

    return   preHour    + std::to_string(hour) + ":" 
           + preMinute  + std::to_string(minute) + ":" 
           + preSeconds + std::to_string(lSeconds); 
}