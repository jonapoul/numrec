#include <string>
#include "global.h"

bool is_in_range(double min, double x, double max, double epsilon) { 
   return (min-epsilon < x && x < max+epsilon); 
}

void parse(const std::string& input, std::string* name, std::string* valuestr) {
   size_t equals = input.find('=');
   if (equals == std::string::npos) {
      printf("You input a parameter without specifying what it is. Whoops!\n");
      exit(1);
   }
   *name     = input.substr(0, equals);
   *valuestr = input.substr(equals+1);
}