#include <string>
#include <algorithm>
#include <fstream>
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

/* returns true if the number x is somewhere in the array, false otherwise */
bool is_in_array(const size_t x, 
                 const std::vector<size_t>& arr) {
   for (const auto a : arr) {
      if (a == x) {
         return true;
      }
   }
   return false;
};

/* used to track down any caught errors. the macro ASSERT() captures a boolean
   statement and passes that here, along with the stringified version of that 
   statement and its file, function and line number */
void my_assert(const bool condition, 
               const char* bool_string, 
               const char* file, 
               const char* function, 
               const int line) {
   if (!condition) {
      printf("\nERROR: Condition [ %s ] evaluated as false\n", bool_string);
      printf("\tfile     = [ %s ]\n",   file);
      printf("\tfunction = [ %s() ]\n", function);
      printf("\tline     = [ %d ]\n\n",   line);
      exit(1);
   }
}

/* captures two integer arguments from command line
      1) image number to use, between 1 and 4 (default = 1)
      2) number of synchronisation runs to complete, at least 1 (default = 20)
*/
void get_arguments(int argc, 
                   char** argv, 
        /*output*/ std::string* filename, 
        /*output*/ size_t* run_limit) {

   int image_number = 1; /* default to desync1.pgm */
   if (argc > 1) {
      ASSERT( isdigit(argv[1][0]) );
      image_number = atoi(argv[1]);
      ASSERT( image_number >= 1 && image_number <= 4 );
   }

   /* build the filepath and test if it's valid */
   char buf[100];
   snprintf(buf, 100, "fft/images/desync%d.pgm", image_number);
   *filename = std::string(buf);
   std::ifstream image_file(buf);
   ASSERT( image_file.is_open() );
   image_file.close();

   *run_limit = 20; /* default to 20 runs, enough to cover all iterations */
   if (argc > 2) {
      ASSERT( isdigit(argv[2][0]) );
      *run_limit = atoi(argv[2]);
      ASSERT( *run_limit >= 1 );
   }
}