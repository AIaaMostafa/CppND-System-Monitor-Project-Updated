#include "processor.h"

#include <thread>
#include <unistd.h>

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  // CPU_Percentage = (totald - idled)/totald
  // float CPU_Percentage = 0.0f;
  // long PrevTotal = LinuxParser::Jiffies();
  // long PrevNonIdle = LinuxParser::ActiveJiffies();
  // std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // long Total = LinuxParser::Jiffies();
  // long NonIdle = LinuxParser::ActiveJiffies();
  // CPU_Percentage = (NonIdle - PrevNonIdle) / (Total - PrevTotal);
  // // return (LinuxParser::ActiveJiffies() / LinuxParser::Jiffies());
  // return CPU_Percentage;
  return (float)(LinuxParser::ActiveJiffies() / sysconf(_SC_CLK_TCK)) /
         LinuxParser::UpTime();
}