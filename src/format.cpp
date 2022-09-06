#include "format.h"

#include <chrono>
#include <iomanip>  // For setw() and setfill()
#include <sstream>
#include <string>

using std::string;

// Format elapsed time (in seconds) to HH:MM:SS
string Format::ElapsedTime(long s) {
  // Initialize second object with the total seconds
  std::chrono::seconds seconds(s);

  // Convert hours from seconds and subtract it from total seconds
  std::chrono::hours hours =
      std::chrono::duration_cast<std::chrono::hours>(seconds);
  seconds -= std::chrono::duration_cast<std::chrono::seconds>(hours);

  // Convert minutes from seconds and subtract it from remaining seconds
  std::chrono::minutes minutes =
      std::chrono::duration_cast<std::chrono::minutes>(seconds);
  seconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

  // Setup string stream to make sure there are always double digits
  std::stringstream ss{};
  ss << std::setw(2) << std::setfill('0') << hours.count() << std::setw(1)
     << ':' << std::setw(2) << std::setfill('0') << minutes.count()
     << std::setw(1) << ':' << std::setw(2) << std::setfill('0')
     << seconds.count();

  return ss.str();
}