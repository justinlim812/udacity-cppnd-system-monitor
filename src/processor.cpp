#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    cpuUtilization_ = LinuxParser::ActiveJiffies() / LinuxParser::Jiffies() * 100.0;
    return cpuUtilization_;
}