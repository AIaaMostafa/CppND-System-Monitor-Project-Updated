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

#define UTIME_INDEX 14u
#define STIME_INDEX 15u
#define CUTIME_INDEX 16u
#define CSTIME_INDEX 17u
#define STARTTIME_INDEX 22u

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
  string line, key, value;
  unsigned int index = 0;
  long utime, stime, cutime, cstime, starttime;
  long total_time, seconds, uptime;
  float cpu_usage;
  uptime = LinuxParser::UpTime();
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(Pid_) +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        switch (index) {
          case UTIME_INDEX:
            utime = atol(value.c_str());
            index++;
            break;
          case STIME_INDEX:
            stime = atol(value.c_str());
            index++;
            break;
          case CUTIME_INDEX:
            cutime = atol(value.c_str());
            index++;
            break;
          case CSTIME_INDEX:
            cstime = atol(value.c_str());
            index++;
            break;
          case STARTTIME_INDEX:
            starttime = atol(value.c_str());
            index++;
            break;

          default:
            index++;
            break;
        }
      }
    }
  }
  total_time = utime + stime + cutime + cstime;
  seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
  cpu_usage = 100 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
  return cpu_usage;
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