#include <string>
#include <algorithm>
#include <limits>

#include "global.h"

bool is_in_range(double min, double x, double max, double epsilon) { 
   return (min-epsilon < x && x < max+epsilon); 
}

void exit_if_false(bool condition, const char* bool_string, const char* file, const char* function, int line) {
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

std::vector<double> generate_smooth_x_values(const double xmin, const double xmax, const double dx) {
   const size_t N = (xmax - xmin) / dx;
   std::vector<double> x_values(N);
   for (size_t i = 0; i < N; i++) {
      x_values[i] = xmin + i*dx;
   }
   return x_values;
}

void fftw_complex_to_vectors(const fftw_complex* c, const size_t N, std::vector<double>* re, std::vector<double>* im) {
   for (size_t i = 0; i < N; i++) {
      re->push_back( c[i][0] );
      im->push_back( c[i][1] );
   }
}

bool is_in_array(const size_t x, const std::vector<size_t>& arr) {
   for (const auto a : arr) if (a == x) return true;
   return false;
};