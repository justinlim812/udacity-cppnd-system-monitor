#include <dirent.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Helper function to help parse value from a specific key
template <typename T>
T findValueByKey(std::string const& keyFilter, std::string const& filename){
  string line;
  string key;
  T value;

  std::ifstream filestream(LinuxParser::kProcDirectory + filename);
  if(filestream.is_open()){
    while(std::getline(filestream,line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == keyFilter){
          filestream.close();
          return value;
        }
      }
    }
  }
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Return a list of processes
// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value, memTotal, memFree;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterMemTotalKey) {
          memTotal = value;
        } 
        else if (key == filterMemFreeKey) {
          memFree = value;
        } 
        else {
          break;
        }
      }
    }
  }

  // Formula: memory utilization = active memory / total memory
  return ((memTotal - memFree) / memTotal);
}

// Read and return the system uptime (in seconds)
long int LinuxParser::UpTime() {
  long int systemUptime{0};
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> systemUptime;
    stream.close();
  }
  return systemUptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line;
  string cpu;
  long jiffies{0}, totalJiffies{0};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    // Discard the first value which is cpu string
    linestream >> cpu;

    for(int i = 0; i < 7; i++){
      linestream >> jiffies;
      totalJiffies += jiffies;
    }
    stream.close();
  }
  return totalJiffies;
}

// Read and return the number of active jiffies for a process
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string dump;
  long activeJiffies{0}, totalActiveJiffies{0};
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    //  Discard whatever values before active jiffies (#14, 15, 16, 17)
    for (int i = 0; i < 13; i++){
      linestream >> dump;
    }
    
    // Store active jiffies in four loops
    for (int j = 0; j < 4; j++){
      linestream >> activeJiffies;
      totalActiveJiffies +=  activeJiffies;
    }
    stream.close();
  }
  return totalActiveJiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies(){
  string line;
  string cpu;
  long activeJiffies{0}, totalActiveJiffies{0};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    // Discard the first value which is cpu string
    linestream >> cpu;

    // Jiffies from user all the way to steal
    for (int i = 0; i < 8; i++){
      linestream >> activeJiffies;

      // Store only jiffies that are not idle and iowait
      if(i != 3 && i != 4){
        totalActiveJiffies += activeJiffies;
      }
    }
    stream.close();
  }
  return totalActiveJiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line;
  string cpu;
  long idleJiffies{0}, totalIdleJiffies{0};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    // Discard the first value which is cpu string
    linestream >> cpu;

    // Discard the jiffies before idle and iowait
    for(int i = 0; i < 5; i++){
      linestream >> idleJiffies;

      // Store only idle and iowait jiffies
      if (i == 3 || i == 4){
        totalIdleJiffies += idleJiffies;
      }
    }
    stream.close();
  }
  return totalIdleJiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string cpu, jiffies;
  vector<string> jiffiesVector;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for(int i = 0; i < 8; i++){
      linestream >> jiffies;
      jiffiesVector.emplace_back(jiffies);
    }
    stream.close();
  }
  return jiffiesVector;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return findValueByKey<int>(filterProcessKey, kStatFilename);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return findValueByKey<int>(filterRunningProcessesKey, kStatFilename);
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string cmd;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    stream.close();
  }

  // Return NULL if there is no command or truncate line if more than 40 characters
  if(line.length()){
    if (line.length() > 40){
      std::string lineTruncated = line.substr(0, 40) + "...";
      return lineTruncated;
    }
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  int ram;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> ram;
      if (key == filterVmRssKey) { // Using VmRSS (Physical RAM) instead of VmSize (Virtual RAM)
        break;
      }
    }
    stream.close();
  }
  return to_string(ram / 1000);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key, userID;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> userID;
      if (key == filterUidKey) {
        break;
      }
    }
    stream.close();
  }
  return userID;
}

// Read and return the user associated with a process (matching process' uid to username)
string LinuxParser::User(int pid) {
  string line;
  string user, x, userID;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> userID;

      // Match the username with process's uid
      if (userID == LinuxParser::Uid(pid)) {
        break;
      }
    }
    stream.close();
  }
  return user;
}

// Read and return the process the process started after system boot (in seconds)
long int LinuxParser::UpTime(int pid) {
  string line;
  string dump;
  long int processStartTime{0};
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    // Process start time at #22
    for (int i = 0; i < 21; i++){
      linestream >> dump;
    }
    linestream >> processStartTime;
    stream.close();
  }

  // Convert process start time from jiffies to seconds
  processStartTime /= sysconf(_SC_CLK_TCK);
  return processStartTime;
}