#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  // initialize all values
  calculateCpuUsage();
  setCommand();
  setRam();
  setUptime();
  setUser();
}

// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return cpu_usage_; }

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram_usage_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }



void Process::setUser() { 
    user_ = LinuxParser::User(Pid()); 
}

void Process::setCommand() { 
    command_ = LinuxParser::Command(Pid()); 
}

// calculate the CPU utilization of this process and save in cpuUsage_
void Process::calculateCpuUsage() {
  // read values from filesystem
  long uptime = LinuxParser::UpTime();
  vector<float> val = LinuxParser::CpuUtilization(Pid());
  // only if the values could be read sucessfully
  if (val.size() == 5) {
    // add utime, stime, cutime, cstime (they are in seconds)
    float totaltime = val[0] + val[1] + val[2] + val[3];
    float seconds = uptime - val[4];
    // calculate the processes CPU usage
    cpu_usage_ = totaltime / seconds;
  } else
    cpu_usage_ = 0;
}

void Process::setRam() {
  string val_kb = LinuxParser::Ram(Pid());

  try {
    long val_mb = std::stol(val_kb) / 1000;
    ram_usage_ = std::to_string(val_mb);
  } catch (const std::invalid_argument& arg) {
    ram_usage_ = "0";
  }
}

void Process::setUptime() {
  uptime_ = LinuxParser::UpTime(Pid());
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return this->pid_ < a.Pid(); 
}