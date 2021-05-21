#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  int TotalMemory = 0;
  int FreeMemory = 0;
  float MemoryUtilization = 0.0;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          TotalMemory = std::stoi(value);
        } else if (key == "MemFree") {
          FreeMemory = std::stoi(value);
          break;
        }
      }
    }
  }
  stream.close();
  MemoryUtilization = (TotalMemory - FreeMemory) / TotalMemory;
  return MemoryUtilization;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string key;
  string value;
  long uptime = 0;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      uptime = std::stol(value);
    }
  }
  stream.close();
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return (LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies());
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { return 0; }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> CPU_utilization = CpuUtilization();
  // NonIdle = user + nice + system + irq + softirq + steal
  return (
      std::stol(CPU_utilization[kUser_]) + std::stol(CPU_utilization[kNice_]) +
      std::stol(CPU_utilization[kSystem_]) + std::stol(CPU_utilization[kIRQ_]) +
      std::stol(CPU_utilization[kSoftIRQ_]) +
      std::stol(CPU_utilization[kSteal_]));
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> CPU_utilization = CpuUtilization();
  // Idle = idle + iowait
  return (std::stol(CPU_utilization[kIdle_]) +
          std::stol(CPU_utilization[kIOwait_]));
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> ret_value;
  string line, key, value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "cpu") {
        while (linestream >> value) {
          ret_value.push_back(value);
        }
        break;
      }
    }
  }
  stream.close();
  return ret_value;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  int total_processes = 0;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        total_processes = std::stoi(value);
        break;
      }
    }
  }
  stream.close();
  return total_processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  int running_processes = 0;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        running_processes = std::stoi(value);
        break;
      }
    }
  }
  stream.close();

  return running_processes;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  stream.close();
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  int ram = 0;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize") {
        ram = std::stoi(value.c_str()) / 1000;
      }
    }
  }
  stream.close();
  return to_string(ram);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  string ret_val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          ret_val = value;
          break;
        }
      }
    }
  }
  stream.close();
  return ret_val;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string key;
  string value;
  string unused_data;
  string user_id;
  user_id = Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> value >> unused_data >> key;
      if (key == user_id) {
        break;
      }
    }
  }
  stream.close();
  return value;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  long up_time = 0;
  int time_position = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value) {
        if (time_position == 21) {
          up_time =
              UpTime() - (std::atol(value.c_str()) / sysconf(_SC_CLK_TCK));
          break;
        }
      }
    }
  }
  stream.close();
  return up_time;
}
