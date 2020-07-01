#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float utilization{0};
  float activeJiffies = static_cast<float>(LinuxParser::ActiveJiffies());
  float idleJiffies = static_cast<float>(LinuxParser::IdleJiffies());

  float activeDiff = activeJiffies - prevActiveJiffies_;
  float idleDiff = idleJiffies - prevIdleJiffies_;

  utilization = activeDiff / (activeDiff + idleDiff);

  prevActiveJiffies_ = activeJiffies;
  prevIdleJiffies_ = idleJiffies;
  return utilization;
}
