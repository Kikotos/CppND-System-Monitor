#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   
  std::vector<Process>& Processes();  
  float MemoryUtilization();          
  long UpTime();                      
  int TotalProcesses();               
  int RunningProcesses();             
  std::string Kernel();               
  std::string OperatingSystem();      

 private:
  void AddNewProcesses(std::vector<int> &pids);
  void RemoveUnusedProcesses(std::vector<int> &pids);

  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif