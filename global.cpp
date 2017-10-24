#include <string>
#include "global.h"

bool is_in_range(double min, double x, double max, double epsilon) { 
   return (min-epsilon < x && x < max+epsilon); 
}

void exit_if_error(bool condition, const char* file, const char* func, int line) {
   if (condition) {
      printf("Error condition triggered in %s, %s(), line %d\n", file, func, line);
      exit(1);
   }
}