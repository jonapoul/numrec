#include <string>
#include <algorithm>

#include "global.h"

bool is_in_range(double min, double x, double max, double epsilon) { 
   return (min-epsilon < x && x < max+epsilon); 
}

void exit_if_false_implementation(bool condition,const char* bool_string,const char* file,const char* function,int line) {
   if (!condition) {
      printf("Error! Condition \"%s\" evaluated as false\n", bool_string);
      printf("file = \"%s\", function = \"%s()\", line = %d\n", file, function, line);
      exit(1);
   }
}

XArray generate_smooth_x_values(const XArray& xinput, const size_t N_points) {
   double xmin = *std::min_element(xinput.begin(), xinput.end());
   double xmax = *std::max_element(xinput.begin(), xinput.end());
   XArray output(N_points, 0.0);
   for (size_t i = 0; i < N_points; i++) {
      output[i] = xmin + i*(xmax-xmin)/(double)N_points;
   }
   return output;
}