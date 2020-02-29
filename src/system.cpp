#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Constructor
System::System(){
  processes_ = System::SetProcesses();
}

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// Compare helper function
bool Compare(const Process a, const Process b){
  return a < b;
}

// Reads PDIs from the /proc directory and returns a container of Processes
// sorted by highest Ram values first
vector<Process> System::SetProcesses() {
  vector<Process> result;
  
  for(int i : LinuxParser::Pids()){
      Process process(i);
      result.push_back(process);
  } 
  std::sort(result.begin(), result.end(), Compare);
  
  return result;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }