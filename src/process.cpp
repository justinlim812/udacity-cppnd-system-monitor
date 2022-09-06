#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// For debugging
#include <iostream>
#define printVariableNameAndValue(x) std::cout<<"The name of variable **"<<(#x)<<"** and the value of variable is => "<<x<<"\n"

// Constructor
Process::Process(int pid) : pid_(pid){}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const{     
    float cpuUtilization{0.0};

    // Time spent in the process
    long processTimeSpent = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);

    // Calculate total system elapsed time after the process started
    // Formula: total system elapsed time = total system runtime - time when process started
    long systemElapsedTimeProcessStarted = LinuxParser::UpTime() - LinuxParser::UpTime(pid_);

    // Formula: 100 * (total time spent in process / total time spent of system since process started)
    cpuUtilization = ((float) processTimeSpent / (float) systemElapsedTimeProcessStarted);
    // printVariableNameAndValue(cpuUtilization);

    return cpuUtilization;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return (a.CpuUtilization() < CpuUtilization());
}