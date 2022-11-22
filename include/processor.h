#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"
#include "vector"

class Processor {
 public:
  float Utilization();

 
 private:
 std::vector<double> prevCpuTimes = std::vector<double>(LinuxParser::CPUStates::kCPUStatesSize_, 0.0);
};

#endif