#include <string>
#include <algorithm>
#include <limits>

#include "global.h"

bool is_in_range(double min, 
                 double x, 
                 double max, 
                 double epsilon) { 
   return (min-epsilon < x && x < max+epsilon); 
}

XArray generate_smooth_x_values(const XArray& xinput, 
                                const size_t N_points) {
   double xmin = *std::min_element(xinput.begin(), xinput.end());
   double xmax = *std::max_element(xinput.begin(), xinput.end());
   XArray output(N_points, 0.0);
   for (size_t i = 0; i < N_points; i++) {
      output[i] = xmin + i*(xmax-xmin)/(double)N_points;
   }
   return output;
}

void fftw_complex_to_vectors(const fftw_complex* c, 
                             const size_t N,
                             std::vector<double>* re, 
                             std::vector<double>* im) {
   for (size_t i = 0; i < N; i++) {
      re->push_back( c[i][0] );
      im->push_back( c[i][1] );
   }
}

bool is_in_array(const size_t x, 
                 const std::vector<size_t>& arr) {
   for (const auto a : arr) if (a == x) return true;
   return false;
};

void my_assert(const bool condition, 
               const char* bool_string, 
               const char* file, 
               const char* function, 
               const int line) {
   if (!condition) {
      printf("ERROR:\n\tCondition [ %s ] evaluated as false\n", bool_string);
      printf("\tfile = [ %s ], function = [ %s() ], line = [ %d ]\n", file, function, line);
      exit(1);
   }
}