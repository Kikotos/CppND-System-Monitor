#include "processor.h"

using LinuxParser::CPUStates;

// Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
    // Create a temporary conteiner
    std::vector<double> cpuTimes(CPUStates::kCPUStatesSize_, 0.0);
    
    // Get parsed data
    cpuTimes = LinuxParser::CpuUtilization();

    // Compute current usage
    double idle = cpuTimes[CPUStates::kIdle_] + cpuTimes[CPUStates::kIOwait_];
    double nonIdle = cpuTimes[CPUStates::kUser_] + cpuTimes[CPUStates::kNice_]  + cpuTimes[CPUStates::kSystem_] 
                   + cpuTimes[CPUStates::kIRQ_] + cpuTimes[CPUStates::kSoftIRQ_] + cpuTimes[CPUStates::kSteal_];
    double total = idle + nonIdle;

    // COmpute previous usage
    double prevIdle = this->prevCpuTimes[CPUStates::kIdle_] + this->prevCpuTimes[CPUStates::kIOwait_];
    double prevNonIdle = this->prevCpuTimes[CPUStates::kUser_] + this->prevCpuTimes[CPUStates::kNice_]  
                       + this->prevCpuTimes[CPUStates::kSystem_] + this->prevCpuTimes[CPUStates::kIRQ_] 
                       + this->prevCpuTimes[CPUStates::kSoftIRQ_] + this->prevCpuTimes[CPUStates::kSteal_];
                       
    double prevTotal = prevIdle + prevNonIdle;

    // Difference
    double dTotal = total - prevTotal;
    double dIdle = idle - prevIdle;

    // Copy to previous
    for(int i{0}; i < static_cast<int>(CPUStates::kCPUStatesSize_); i++)
    {
        this->prevCpuTimes[i] = cpuTimes[i];
    }

    return static_cast<float>((dTotal - dIdle)/dTotal); 
}