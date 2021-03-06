#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>

#include "global.h"

void my_starting(const char* func) {
   printf("%sSTARTING %s()%s\n", CYAN, func, RESET);
}

void my_ending(const char* func) {
   printf("%sENDING %s()%s\n\n", CYAN, func, RESET);
}

void my_exit(const char* file,
             const char* function,
             const int line) {
   printf(RED "\n\tEXIT() called!\n" RESET);
   printf("\tfile     = [ %s ]\n",   file);
   printf("\tfunction = [ %s() ]\n", function);
   printf("\tline     = [ %d ]\n\n", line);
   exit(1);
}

void print_vector(const std::vector<std::string>& vec) {
   for (size_t i = 0; i < vec.size(); i++) {
      std::cout << i << '/' << vec.size()-1 << ' ' << vec[i] << '\n';
   }
   printf("\n");
}

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

/* used to track down any caught errors. the macro ASSERT() captures a boolean
   statement and passes that here, along with the stringified version of that 
   statement and its file, function and line number */
void my_assert(const bool condition, 
               const char* bool_string, 
               const char* file, 
               const char* function, 
               const int line) {
   if (!condition) {
      printf(RED "\nERROR: Condition [ %s ] evaluated as false\n", bool_string);
      printf(YELLOW);
      printf("\tfile     = [ %s ]\n",   file);
      printf("\tfunction = [ %s() ]\n", function);
      printf("\tline     = [ %d ]\n\n",   line);
      printf(RESET);
      exit(1);
   }
}

/* captures three integer arguments from command line
      1) image number to use, between 1 and 4 (default = 1)
      2) number of synchronisation runs to complete, at least 1 (default = 20)
      3) 0 = simple output, 1 = verbose output
*/
void get_arguments(int argc, 
                   char** argv, 
        /*output*/ std::string* filename, 
        /*output*/ size_t* run_limit,
        /*output*/ bool* print_debug) {
   if (argc == 1) {
      printf(GREEN);
      printf("\nArguments:\n");
      printf("\t1: image number   = integer from 1-4 (default=1)\n");
      printf("\t2: iteration limt = integer >= 1     (default=20)\n");
      printf("\t3: verbose output = 1 or 0           (default=0)\n");
      printf("e.g. running \"%s 2 10 1\" ", argv[0]);
      printf("will read desync2.pgm, run the synchronisation for 10 "); 
      printf("iterations and print debug output\n\n");
      printf(RESET);
   }

   int first_argument = 1; /* default to desync1.pgm */
   if (argc > 1) {
      ASSERT( isdigit(argv[1][0]) );
      first_argument = atoi(argv[1]);
      ASSERT( first_argument >= 1 && first_argument <= 4 );
   }
   /* build the filepath and test whether it's valid */
   char buf[100];
   snprintf(buf, 100, "fft/images/desync%d.pgm", first_argument);
   *filename = std::string(buf);
   std::ifstream image_file(buf);
   ASSERT( image_file.is_open() );
   image_file.close();

   *run_limit = 20; /* default to 20 runs, enough to cover all iterations */
   if (argc > 2) {
      ASSERT( isdigit(argv[2][0]) );
      const int second_argument = atoi(argv[2]);
      ASSERT( second_argument >= 1 );
      *run_limit = second_argument;
   }

   *print_debug = false; /* default to not printing verbose output */
   if (argc > 3) {
      ASSERT( isdigit(argv[3][0]) );
      const int third_argument = atoi(argv[3]);
      ASSERT( third_argument == 0 || third_argument == 1 );
      *print_debug = (bool)third_argument;
   }
}