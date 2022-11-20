#include "processor.h"

// Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
    // Create a temporary conteiner
    std::vector<double> cpuTimes(kCPUStatesSize_, 0.0);
    
    // Get parsed data
    cpuTimes = LinuxParser::CpuUtilization();

    // Compute current usage
    double idle = cpuTimes[kIdle_] + cpuTimes[kIOwait_];
    double nonIdle = cpuTimes[kUser_] + cpuTimes[kNice_]  + cpuTimes[kSystem_] 
                   + cpuTimes[kIRQ_] + cpuTimes[kSoftIRQ_] + cpuTimes[kSteal_];
    double total = idle + nonIdle;

    // COmpute previous usage
    double prevIdle = this->prevCpuTimes[kIdle_] + this->prevCpuTimes[kIOwait_];
    double prevNonIdle = this->prevCpuTimes[kUser_] + this->prevCpuTimes[kNice_]  
                       + this->prevCpuTimes[kSystem_] + this->prevCpuTimes[kIRQ_] 
                       + this->prevCpuTimes[kSoftIRQ_] + this->prevCpuTimes[kSteal_];
                       
    double prevTotal = prevIdle + prevNonIdle;

    // Difference
    double dTotal = total - prevTotal;
    double dIdle = idle - prevIdle;

    // Copy to previous
    for(int i{0}; i < static_cast<int>(kCPUStatesSize_); i++)
    {
        this->prevCpuTimes[i] = cpuTimes[i];
    }

    return static_cast<float>((dTotal - dIdle)/dTotal); 
}