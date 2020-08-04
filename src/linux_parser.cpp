#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;
using std::map;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line, key, value = "";
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
  string os, kernel, version, line;
  std::ifstream stream(kProcDirectory + kVersionFilename); // open file content stream
  if (stream.is_open()) {   // Is the stream open? Can I use it? 
    std::getline(stream, line);  // can do without a while because there's only one line
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

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
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string memtotal, memfree, rowname, line;
  long memtotal_l, memfree_l;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if (stream.is_open()) { 
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> rowname >> memtotal;
    std::getline(stream, line);
    linestream >> rowname >> memfree;
    memtotal_l = stol(memtotal);
    memfree_l = stol(memfree);
    return (memtotal_l - memfree_l)/((float) memtotal_l); 
  }

  return 0.0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename); // open file content stream
  if (stream.is_open()) {   // Is the stream open? Can I use it? 
    std::getline(stream, line);  // can do without a while because there's only one line
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// DONE: Read and return CPU utilization
map<string, long> LinuxParser::CpuUtilization() { 
  string line;
  string user, nice, sys, idle, iowait, irq, softirq, steal, guest, guest_nice;
  map<string, long> res;
  std::ifstream stream(kProcDirectory + kCpuinfoFilename); // open file content stream
  if (stream.is_open()) {   // Is the stream open? Can I use it? 
    std::getline(stream, line);  // can do without a while because there's only one line
    std::istringstream linestream(line);
    linestream >> user >> nice >> sys >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    res["idle"] = stol(idle);
    res["iowait"] = stol(iowait);
    res["user"] = stol(user) - stol(guest);
    res["nice"] = stol(nice) - stol(guest_nice);
    res["sys"] = stol(sys);
    res["irq"] = stol(irq);
    res["softirq"] = stol(softirq);
    res["steal"] = stol(steal);
    return res;
  }
  return res;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, value, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return stoi(value);
        }
      }
    }
  }
  return 0; 
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key, value, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
  return 0; 
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string value = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, value);
    return value;
  }
  return value;

}


#define CPU_UTIME 14
#define CPU_KTIME 15
#define CPU_CUTIME 16
#define CPU_CSTIME 17
#define START_TIME 22

// Read and return the CPU usage of a process
// return the values in seconds
vector<float> LinuxParser::CpuUtilization(int pid) {
  vector<float> cpuValues{};
  string line, value;
  float time = 0.f;
  int i;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (i = 1; i <= START_TIME; i++) {
        linestream >> value;
        switch(i) {
          case CPU_UTIME: 
          case CPU_KTIME: 
          case CPU_CUTIME: 
          case CPU_CSTIME: 
          case START_TIME:
            time = std::stof(value) / sysconf(_SC_CLK_TCK);
            cpuValues.push_back(time);
        }
      }
    }
  }
  return cpuValues;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  // read the user ID for this process
  string uid = Uid(pid);
  string line;
  string key;
  string value = "";
  string other;
  // find user name for this user ID in /etc/passwd
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> other >> key) {
        if (key == uid) {
          return value;
        }
      }
    }
  }
  return value;
}


// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  long uptime = 0;
  string value;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i <= START_TIME; i++) {
        linestream >> value;
        if (i == START_TIME) {
          // read the starttime value in clock ticks and convert to seconds
          // devide by clock ticks per second
          try {
            uptime = std::stol(value) / sysconf(_SC_CLK_TCK);
            return uptime;
          } catch (const std::invalid_argument& arg) {
            return 0;
          }
        }
      }
    }
  }
  return uptime;
}
