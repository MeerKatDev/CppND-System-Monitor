#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  
  int Pid() const;                               
  std::string User();                    
  std::string Command();                 
  float CpuUtilization();                
  std::string Ram();                     
  long int UpTime();                     
  bool operator<(Process const& a) const;

 private:
  int pid_;
  float cpu_usage_;
  std::string command_;
  std::string ram_usage_;
  std::string user_;
  long int uptime_;

  void setUser();
  void setCommand();
  void calculateCpuUsage();
  void setRam();
  void setUptime();
  
};

#endif