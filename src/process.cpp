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

// Process Class Constructor
Process::Process(int pid) : pid_(pid) { cpuUtilization_ = CpuUtilization(); };
// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  // total elapsed time in seconds since the process started
  // seconds = uptime - (starttime/Hertz) = sys_uptime -
  // process_uptime_in_seconds
  float sys_uptime = static_cast<float>(LinuxParser::UpTime());
  float process_uptime = static_cast<float>(UpTime());
  float Hertz = static_cast<float>(sysconf(_SC_CLK_TCK));
  float seconds = sys_uptime - process_uptime;

  // CPU usage percentage
  // cpu_usage = 100 * ((total_time/Hertz) / seconds);
  if (seconds != 0)  // to avoid division by zero
    return (
        1.0 *
        (static_cast<float>(LinuxParser::ActiveJiffies(this->Pid())) / Hertz) /
        seconds);
  else
    return 0;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process cpu
// utilization
bool Process::operator<(Process const& a) const {
  return this->cpuUtilization_ > a.cpuUtilization_;
}
