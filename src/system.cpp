#include <unistd.h>
#include <cstddef>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
std::vector<Process>& System::Processes() 
{ 
    vector<int> pids = LinuxParser::Pids();

    // Add new processes
    AddNewProcesses(pids);

    // Remove unused
    RemoveUnusedProcesses(pids);

    // Sort processes
    std::sort(processes_.begin(), processes_.end());

    return processes_; 
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization() ; }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses() ; }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }



void System::AddNewProcesses(vector<int> &pids)
{
    // Check every PID
    for(int &pid: pids)
    {
        // Find the PID
        std::vector<Process>::iterator findIter = std::find(processes_.begin(), processes_.end(), pid);

        // If it's not found then push the new one
        if(findIter == processes_.end())
        {
            Process newProcess(pid);
            processes_.push_back(newProcess);
        }
    }
}

void System::RemoveUnusedProcesses(vector<int> &pids)
{
    vector<int> removePid;
    // Check every PID
    for(Process &proc: processes_)
    {
        // Find the PID
        std::vector<int>::iterator findIter = std::find(pids.begin(), pids.end(), proc.Pid());

        // If it's not found then push the new one
        if(findIter == pids.end())
        {
            removePid.push_back(proc.Pid());
        }
    }

    // Remove pids
    for(int &pid: removePid)
    {
        // Find the PID
        std::vector<Process>::iterator findIter = std::find(processes_.begin(), processes_.end(), pid);

        // If it's not found then push the new one
        if(findIter != processes_.end())
        {
            processes_.erase(findIter);
        }
    }
}