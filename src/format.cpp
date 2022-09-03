#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long times) { 
    int hours, minutes, seconds;

    //  Get total hours
    hours = times / (60 * 60);

    //  Get leftover minutes by using total uptime minus hours (in seconds)
    minutes = (times - hours * (60 * 60)) / 60;

    // Get leftover minutes by using total uptime minus hours and minutes (in seconds)
    seconds =  times - hours * (60 * 60) - minutes * 60;

    string str = std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
    return str;
}