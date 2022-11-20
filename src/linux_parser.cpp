#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "linux_parser.h"


using std::stof;
using std::string;
using std::to_string;
using std::vector;

//*********************************** SYSTEM **************************
// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  string line;
  string key, value,units;
  float memTotal, memFree = 0.0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      while (linestream >> key >> value >> units) 
      {
        if (key == "MemTotal") 
        {
          memTotal = stof(value);
        }
        if (key == "MemFree") 
        {
          memFree = stof(value);
        }
      }
    }
  }

  return (1.0 - ((memTotal - memFree)/memTotal));
}


// Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  string line;
  string value, idleTime;
  long upTime= 0;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);

      while (linestream >> value >> idleTime) 
      {
         upTime = long(stof(value));
      }
    }
  }

  return upTime;
  
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  string line;
  string key, value;
  int procNum = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);

      while (linestream >> key) 
      {
        if (key == "processes") 
        {
          while (linestream >> value) 
          {
              procNum = stoi(value);
          }
        }
      }
    }
  }

  return procNum;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{ 
  string line;
  string key, value;
  int procNum = 0;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);

      if (linestream >> key) 
      {
        if (key == "procs_running") 
        {
          if (linestream >> value) 
          {
              procNum = stoi(value);
          }
        }
      }
    }
  }

  return procNum;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}



//*********************************** CPU **************************

// TODO: Read and return CPU utilization
vector<double> LinuxParser::CpuUtilization() 
{ 
  vector<double> cpuTimes{0};

  string line;
  string key, value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);

      if (linestream >> key) 
      {
        if (key == "cpu") 
        {
          while (linestream >> value) 
          {
              cpuTimes.push_back(stod(value));
          }
        }
      }
    }
  }

  return cpuTimes; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }



//*********************************** PROCESSES **************************

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{ 
  string line;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
  }

  return line; 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{ 
  string line;
  string key, memory;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      if (linestream >> key) 
      {
        if (key == "VmSize") 
        {
          if (linestream >> memory) 
          {
              break;
          }
        }
      }
    }
  }

  return memory;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{ 
  string line;
  string key, uid;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      if (linestream >> key) 
      {
        if (key == "Uid") 
        {
          if (linestream >> uid) 
          {
              break;
          }
        }
      }
    }
  }

  return uid; 
}

// Read and return the user associated with a process
string LinuxParser::User(int iUid) 
{ 
  string line;
  string key, x, uid;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      if (linestream >> key >> x >> uid) 
      {
        if (stoi(uid) == iUid) 
        {
            break;
        }
      }
    }
  }

  return key; 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) 
{ 
  string line;
  string value;
  long long startTimeJiffies = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) 
  {
    if(std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);

      int valuePos = 1;

      while (linestream >> value) 
      {
        if (valuePos == 22) 
        {
            startTimeJiffies = stoll(value);
            break;
        }

        valuePos++;
      }
    }
  }

  return static_cast<long>((startTimeJiffies > 0) ? startTimeJiffies/sysconf(_SC_CLK_TCK) : 0); 
}

// Read and return the number of active time in seconds for a PID
float LinuxParser::ActiveTime(int pid) 
{ 
  
  string line;
  string value;
  long long timeJiffies = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) 
  {
    if(std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);

      int valuePos = 1;

      while (linestream >> value) 
      {
        if ((valuePos >= 14) && (valuePos <= 17)) 
        {
            timeJiffies += stoll(value);

            if(valuePos == 17)
            {
              break;
            }
        }

        valuePos++;
      }
    }
  }

  return (timeJiffies > 0) ? static_cast<float>(timeJiffies)/static_cast<float>(sysconf(_SC_CLK_TCK)) : 0.0;
  
}