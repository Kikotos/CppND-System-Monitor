#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid) : pid(pid){}

// Return this process's ID
int Process::Pid() const { return pid; }

// TODO: Return this process's CPU utilizationprocesses
float Process::CpuUtilization() 
{ 
    float activeTime = LinuxParser::ActiveTime(pid);
    float processUpTime = static_cast<float>(LinuxParser::UpTime(pid));
    float systemUpTime = static_cast<float>(LinuxParser::UpTime());

    cpuUtilization = ((activeTime > 0) && (processUpTime > 0) && (systemUpTime>0)) ? activeTime/(systemUpTime-processUpTime) : 0.0;
    
    return cpuUtilization; 
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// Return this process's memory utilization
string Process::Ram() 
{ 
    string memoryKb = LinuxParser::Ram(pid);
    string memoryMb;

    if(!memoryKb.empty())
    {
        memoryMb = to_string(stol(memoryKb)/1024);
    }

    return memoryMb; 
}

// TODO: Return the user (name) that generated this process
string Process::User() 
{ 
    string userName;
    string uid = LinuxParser::Uid(pid);

    if(!uid.empty())
    {
        userName = LinuxParser::User(stoi(uid));
    }

    return userName;   
}

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const 
{ 
    return pid < a.pid; 
}


bool Process::operator== (Process const &a) const
{
    return pid == a.pid;
}