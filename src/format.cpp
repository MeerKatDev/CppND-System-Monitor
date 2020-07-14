#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours = seconds / 3600;
    int remaining = seconds - 3600 * hours;
    int minutes = remaining / 60;
    remaining -= minutes * 60;
    string hh = (hours <= 9 ? "0" : "")   + to_string(hours);
    string mm = (minutes <= 9 ? "0" : "") + to_string(minutes);
    string ss = to_string(remaining);
    return hh + ":" + mm + ":" + ss; 

}