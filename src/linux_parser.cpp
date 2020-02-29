#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// An example of how to read data from the filesystem
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

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Reads PIDs from /proc directory
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memUtil;
  float memTotal, memFree;
  string sMemTotal, sMemFree;
  string key, value, kb;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value >> kb;
      if(key == "MemTotal:") { sMemTotal = value; }
      if(key == "MemFree:") { sMemFree = value; }
    }
  }
  
  memTotal = stof(sMemTotal);
  memFree = stof(sMemFree);
  memUtil = memTotal - memFree;
  
  return memUtil/memTotal; 
}

// Read and return the system uptime
long int LinuxParser::UpTime() {
  string sUptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> sUptime;
  }
  
  long int uptime = stol(sUptime);
  return uptime; 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  vector<string> statContents = StatFileContents(pid);
  
  long utime = stol(statContents[13]);
  long stime = stol(statContents[14]);
  long cutime = stol(statContents[15]);
  long cstime = stol(statContents[16]);
  
  return utime + stime + cutime + cstime;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long user, nice, system, irq,
    softirq, steal, guest, guest_nice;

  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    vector<string> jiffies((std::istream_iterator<string>(linestream)),
                           std::istream_iterator<string>()); 

    user = stol(jiffies[1]);
    nice = stol(jiffies[2]);
    system = stol(jiffies[3]);
    irq = stol(jiffies[6]);
    softirq = stol(jiffies[7]);
    steal = stol(jiffies[8]);
    guest = stol(jiffies[9]);
    guest_nice = stol(jiffies[10]);

    return user + nice + system + irq +
      softirq + steal + guest + guest_nice;
  }
  
  return 0;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idle, iowait;

  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {

    std::getline(stream, line);
    std::istringstream linestream(line);
    vector<string> jiffies((std::istream_iterator<string>(linestream)),
                           std::istream_iterator<string>()); 

    idle = stol(jiffies[4]);
    iowait = stol(jiffies[5]);

    return idle + iowait;
  }
  
  return 0;
} 

// Read and return CPU utilization
float LinuxParser::CpuUtilization() { 
  float totalJ, idleJ, activeJ;
  
  idleJ = LinuxParser::IdleJiffies();
  activeJ = LinuxParser::ActiveJiffies();
  totalJ = idleJ + activeJ;
  std::cout << activeJ/totalJ;

  return activeJ/totalJ; 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          int totalProc = std::stoi(value);
          return totalProc;
        }
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          int runningProcs = std::stoi(value);
          return runningProcs;
        }
      }
    }
  }
  return 0;
}

// Returns a vector with all the contents of a given pid stat file
std::vector<string> LinuxParser::StatFileContents(int pid){
  string line;
  string sPid = "/" + to_string(pid);
  std::ifstream stream(kProcDirectory + sPid + kStatFilename);
  getline(stream, line);
  std::istringstream linestream(line);
  
  vector<string> contents((std::istream_iterator<string>(linestream)),
                          std::istream_iterator<string>());
  
  return contents;
}

// Read and return CPU utilization for a PID
float LinuxParser::CpuUtilization(int pid) { 
  vector<string> statContents = StatFileContents(pid);
  long starttime = stol(statContents[21]);
  long totalproctime = LinuxParser::ActiveJiffies(pid);
  long uptime = LinuxParser::UpTime();
  
  float seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
  
  float cpu_usage = (totalproctime / sysconf(_SC_CLK_TCK)) / seconds;
  
  return cpu_usage;
}


// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmdLine;
  string sPid = "/" + to_string(pid);
  std::ifstream stream(kProcDirectory + sPid + kCmdlineFilename);
  
  std::getline(stream, cmdLine);
  return cmdLine; 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string Ram;
  string key, value;
  string line;
  string sPid = "/" + to_string(pid);
  std::ifstream stream(kProcDirectory + sPid + kStatusFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:") { Ram = value; }
    }
  }
  
  int ramInt = stoi(Ram);
  Ram = to_string(ramInt/1000);
  
  return Ram; 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, value;
  string line;
  string sPid = "/" + to_string(pid);
  std::ifstream uidStream(kProcDirectory + sPid + kStatusFilename);
  
  if (uidStream.is_open()) {
    while (std::getline(uidStream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string username, x, uid;
  string line;
  std::ifstream usernameStream(kPasswordPath);
  
  string uidCompare = LinuxParser::Uid(pid);
  
  if (usernameStream.is_open()) {
    while (std::getline(usernameStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> username >> x >> uid){
        if (uid == uidCompare){
          return username;
        }
      }
    }
  }
  return string(); 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  vector<string> statContents = StatFileContents(pid);
  long clockTicks = stol(statContents[21]);
  long uptime = clockTicks/sysconf(_SC_CLK_TCK);
  return uptime; 
}
