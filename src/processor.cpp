#include "processor.h"

#include "linux_parser.h"

// Return the aggregate system utilization
float Processor::Utilization() {
  // Formula: cpu utilization = activeJIffies / totalJiffies
  // ncusesdisplay already multiply the value by 100, so multiplication is not
  // needed here
  Utilization_ =
      (float)LinuxParser::ActiveJiffies() / (float)LinuxParser::Jiffies();
  return Utilization_;
}