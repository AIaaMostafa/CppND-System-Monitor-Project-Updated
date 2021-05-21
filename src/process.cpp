#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int process_num) {
  Pid_ = process_num;
  Command_ = LinuxParser::Command(Pid_);
  Ram_ = LinuxParser::Ram(Pid_);
  User_ = LinuxParser::User(Pid_);
  UpTime_ = LinuxParser::UpTime(Pid_);
}

// Return this process's ID
int Process::Pid() { return Pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  return LinuxParser::CpuUtilization(Pid_);
}

// Return the command that generated this process
string Process::Command() { return Command_; }

//  Return this process's memory utilization
string Process::Ram() { return Ram_; }

//  Return the user (name) that generated this process
string Process::User() { return User_; }

//  Return the age of this process (in seconds)
long int Process::UpTime() { return UpTime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (CpuUtilization() < a.CpuUtilization());
}