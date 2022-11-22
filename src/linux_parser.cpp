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
using std::pair;

//*********************************** STATIC **************************
// Parse file with keys
static vector<string> 
ParseFileKeys(const string &fileLocation, vector<char> &unusedSymbols, vector<pair<string, int>> &keysLine)
{
  string line;
  string fileKey;
  string fileValue;
  vector<string> parsedValues;

  std::ifstream filestream(fileLocation);
  if (filestream.is_open()) 
  {
	// Check each line
    while (std::getline(filestream, line)) 
    {
		// Remove symbols
      for(const char &symbol: unusedSymbols)
      {
        std::replace(line.begin(), line.end(), symbol, ' ');
      }
      
      std::istringstream linestream(line);

		// Get the key
      if (linestream >> fileKey) 
      {
		// Check if the key is in the list
		for(auto key: keysLine)
		{
			if((key.first == fileKey) || (key.first.empty()))
			{	
				// If there is no key then push key as well
				if(key.first.empty())
				{
					parsedValues.push_back(fileKey);
				}

				// Get value
				while ((linestream >> fileValue) && (key.second-- > 0)) 
				{
					parsedValues.push_back(fileValue);
				}
				break;
			}
		}
      }
    }
  }

  return parsedValues;
}

// Parse file with position in file
static vector<string> 
ParseFilePosition(const string &fileLocation, const vector<char> &unusedSymbols, const vector<int> &positionInLine)
{
  string line;
  string fileValue;
  vector<string> parsedValues;

  std::ifstream filestream(fileLocation);
  if (filestream.is_open()) 
  {
	// Check each line
    while (std::getline(filestream, line)) 
    {
		// Remove symbols
      for(const char &symbol: unusedSymbols)
      {
        std::replace(line.begin(), line.end(), symbol, ' ');
      }
      
      std::istringstream linestream(line);
	  
	  int position = 0;

		// Get the key
      while (linestream >> fileValue) 
      {
		// Check if the key is in the list
		for(int pos: positionInLine)
		{
			if(position == pos)
			{	
				parsedValues.push_back(fileValue);
			}
		}

		position++;
      }
    }
  }

  return parsedValues;
}


//*********************************** SYSTEM **************************
// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
	vector <char> excludeSymbols{':'};
	vector <pair<string,int>> keys{{"MemTotal",1}, {"MemFree",1}};

	vector <string> parsedValue = ParseFileKeys(kProcDirectory + kMeminfoFilename, 
												excludeSymbols, keys);

	float memTotal, memFree = 0.0;

	memTotal = stof(parsedValue[0]);
	memFree = stof(parsedValue[1]);

    return (1.0 - ((memTotal - memFree)/memTotal)); 
}


// Read and return the system uptime
long LinuxParser::UpTime() 
{ 
	vector <char> excludeSymbols;
	vector <pair<string,int>> keys{{"",2}};

	vector <string> parsedValue = ParseFileKeys(kProcDirectory + kUptimeFilename, 
												excludeSymbols, keys);

    return static_cast<long>((stof(parsedValue[0])));
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
	vector <char> excludeSymbols;
	vector <pair<string,int>> keys{{"processes",1}};

	vector <string> parsedValue = ParseFileKeys(kProcDirectory + kStatFilename, 
												excludeSymbols, keys);

    return stoi(parsedValue[0]);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{ 
	vector <char> excludeSymbols;
	vector <pair<string,int>> keys{{"procs_running",1}};

	vector <string> parsedValue = ParseFileKeys(kProcDirectory + kStatFilename, 
												excludeSymbols, keys);

    return stoi(parsedValue[0]);
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
   	vector <char> excludeSymbols;
	vector <pair<string,int>> keys{{"",3}};

	vector <string> parsedValue = ParseFileKeys(kProcDirectory + kVersionFilename, 
												excludeSymbols, keys);

  	return parsedValue[2];
}



//*********************************** CPU **************************

// Read and return CPU utilization
vector<double> LinuxParser::CpuUtilization() 
{ 
  	vector<double> cpuTimes{0};

	vector <char> excludeSymbols;
	vector <pair<string,int>> keys{{"cpu",static_cast<int>(kCPUStatesSize_)}};

	vector <string> parsedValue = ParseFileKeys(kProcDirectory + kStatFilename, 
												excludeSymbols, keys);

	for(const string &value: parsedValue)
	{
		cpuTimes.push_back(stod(value));
	}

    return cpuTimes; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
	
	(void)pid;
	return 0; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// Read and return the number of idle jiffies for the system
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
	vector <char> excludeSymbols{':'};
	vector <pair<string,int>> keys{{"VmSize",1}};

	vector <string> parsedValue = ParseFileKeys(kProcDirectory + to_string(pid) + kStatusFilename, 
												excludeSymbols, keys);

    return (!parsedValue.empty()) ? parsedValue.front() : std::string(); 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{
	vector <char> excludeSymbols{':'};
	vector <pair<string,int>> keys{{"Uid",1}};

	vector <string> parsedValue = ParseFileKeys(kProcDirectory + to_string(pid) + kStatusFilename, 
												excludeSymbols, keys);

    return (!parsedValue.empty()) ? parsedValue.front() : std::string(); 
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
    vector <char> excludeSymbols;
	vector <int> position{21};
	long long timeJiffies = 0;

	vector <string> parsedValue = ParseFilePosition(kProcDirectory + to_string(pid) + kStatFilename, 
													excludeSymbols, position);

	for(string &value: parsedValue)
	{
		timeJiffies += stoll(value);
	}

  	return (timeJiffies > 0) ? static_cast<float>(timeJiffies)/static_cast<float>(sysconf(_SC_CLK_TCK)) : 0.0; 
}

// Read and return the number of active time in seconds for a PID
float LinuxParser::ActiveTime(int pid) 
{ 
    vector <char> excludeSymbols;
	vector <int> position{13,14,15,16};
	long long timeJiffies = 0;

	vector <string> parsedValue = ParseFilePosition(kProcDirectory + to_string(pid) + kStatFilename, 
													excludeSymbols, position);

	for(string &value: parsedValue)
	{
		timeJiffies += stoll(value);
	}

  	return (timeJiffies > 0) ? static_cast<float>(timeJiffies)/static_cast<float>(sysconf(_SC_CLK_TCK)) : 0.0; 
}