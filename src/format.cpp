#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hrs;
  int min;
  int sec;
  string elapsedTime;
  min = seconds / 60;
  sec = seconds % 60;
  hrs = min / 60;
  min = min % 60;

  if (hrs < 10) elapsedTime += "0";
  elapsedTime += std::to_string(hrs) + ":";
  if (min < 10) elapsedTime += "0";
  elapsedTime += std::to_string(min) + ":";
  if (sec < 10) elapsedTime += "0";
  elapsedTime += std::to_string(sec);

  return elapsedTime;
}
