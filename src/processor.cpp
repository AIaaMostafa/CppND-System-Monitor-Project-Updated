#include "processor.h"

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  // CPU_Percentage = (totald - idled)/totald
  return (LinuxParser::ActiveJiffies() / LinuxParser::Jiffies());
}