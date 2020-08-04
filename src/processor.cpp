#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <map>
#include <sstream>

using std::string;
using std::map;

float Processor::Utilization() { 
  map<string, long> cpu_data = LinuxParser::CpuUtilization();
  if(cpu_data.find("idle") == cpu_data.end()) return 0.0;

  long idle = cpu_data["idle"] + cpu_data["iowait"];
  long nonidle = cpu_data["user"] + cpu_data["nice"] + cpu_data["sys"] + cpu_data["irq"] + cpu_data["softirq"] + cpu_data["steal"];

  return nonidle/ (float) (idle + nonidle);
}