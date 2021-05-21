#include "format.h"

#include <string>

using std::string;
using std::to_string;
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int HH, MM, SS;
  HH = seconds / 3600;
  seconds %= 3600;
  MM = seconds / 60;
  SS = seconds % 60;
  return to_string(HH) + ":" + to_string(MM) + ":" + to_string(SS);
}