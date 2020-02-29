#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  Process(int id);
  int Pid();                     
  std::string User();
  std::string Command(); 
  float CpuUtilization();
  std::string Ram();
  long int UpTime();                       
  bool operator<(Process const& a) const;  
  bool Compare(Process const a, Process const b);

 private:
    int pid_;
};

#endif