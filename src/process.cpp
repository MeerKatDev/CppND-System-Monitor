#include <unistd.h>
#include <cctype>
#include <iostream>
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
  command_ = LinuxParser::Command(Pid());
  uptime_ = LinuxParser::UpTime(Pid());
  user_ = LinuxParser::User(Pid());

  cpu_usage_ = calculateCpuUsage();
  ram_usage_ = calculateRamUsage();
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

// calculate the CPU utilization of this process and save in cpuUsage_
float Process::calculateCpuUsage() {
  map<string, float> val = LinuxParser::CpuUtilization(Pid());
  if (val.size() == 5)
    return (val["utime"] + val["ktime"] + val["cutime"] + val["cstime"]) /
           (uptime_ - val["stime"]);
  else
    return 0;
}

string Process::calculateRamUsage() {
  string val_kb = LinuxParser::Ram(Pid());

  try {
    long val_mb = std::stol(val_kb) / 1000;
    return std::to_string(val_mb);
  } catch (const std::invalid_argument& arg) {
    std::cout << "error in Process::calculateRamUsage" << std::endl;
    return "0";
  }
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return this->pid_ < a.Pid(); }