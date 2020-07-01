#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >>
        kernel;  // modified to get the correct kernel string
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value;
  string kB;
  float memTotal{1.0};
  float memFree{0.0};
  float memUtil{0.0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> kB) {
        if (key == "MemTotal:")
          memTotal = value;
        else if (key == "MemFree:")
          memFree = value;
      }

      // if the needed values gets updated, calculate the memUtilization and
      // break from the while loop
      if (memTotal != 1 && memFree != 0) {
        memUtil = ((memTotal - memFree) / static_cast<float>(memTotal));
        break;
      }
    }
  }
  return memUtil;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string value;
  string uptime;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> uptime;
    }
  }
  return stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long totalJiffies{0};

  // add all jiffies in cpuutilization (kUser_ ,kNice_,kSystem_,
  // kIdle_,kIOwait_,kIRQ_,kSoftIRQ_,kSteal_,kGuest_,kGuestNice_)
  for (string jiffy : CpuUtilization()) {
    totalJiffies += stol(jiffy);
  }
  return totalJiffies;
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  int count{0};
  string value;
  long utime{0};
  long stime{0};
  long cutime{0};
  long cstime{0};

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while ((linestream >> value) && count != 12) {
        count++;
      }
      linestream >> utime >> stime >> cutime >> cstime;
    }
  }

  // total time spent for the process including time from children processes
  return (utime + stime + cutime + cstime);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // Idle = idle + iowait
  return stol(CpuUtilization()[CPUStates::kIdle_]) +
         stol(CpuUtilization()[CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuUtil;
  string line;
  string cpu;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> cpu >> value) cpuUtil.push_back(value);
      while (linestream >> value) cpuUtil.push_back(value);
    }
  }
  // cpuUtil contain values for { kUser_ ,kNice_,kSystem_,
  // kIdle_,kIOwait_,kIRQ_,kSoftIRQ_,kSteal_,kGuest_,kGuestNice_}
  return cpuUtil;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  int processes{0};

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") processes = value;
      }
    }
  }
  return processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  int runningProcesses{0};

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") runningProcesses = value;
      }
    }
  }
  return runningProcesses;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string cmd;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      cmd = line;
    }
  }
  return cmd;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  string ram;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:")
          ram = to_string(std::stol(value) /
                          1024);  // convert ram from kb into Mb
      }
    }
  }
  return ram;
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  string uid;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") uid = value;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string key;
  string x;
  string value;
  string userName;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> x >> value) {
        if (Uid(pid) == value) {  // compare the parsed uid value with the uid
                                  // of the current process
          userName = key;
        }
      }
    }
  }
  return userName;
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  int count{0};
  string value;
  long uptime;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while ((linestream >> value) && count != 20) {
        count++;
      }
      linestream >> uptime;
    }
  }
  // convert uptime value from ticks to seconds
  return uptime / sysconf(_SC_CLK_TCK);
}
