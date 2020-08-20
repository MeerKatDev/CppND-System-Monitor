#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using std::string;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);

  int Pid() const;
  string User();
  string Command();
  float CpuUtilization();
  string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_;
  float cpu_usage_;
  string command_;
  string ram_usage_;
  string user_;
  long int uptime_;

  float calculateCpuUsage();
  string calculateRamUsage();
};

#endif