#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float prevActiveJiffies_{0.0};
  float prevIdleJiffies_{0.0};
};

#endif
