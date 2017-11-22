#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <vector>
#include <string>
#include <fftw3.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

/*
   MACHINE
*/
#define STARTING() my_starting(__FUNCTION__);
#define ENDING() my_ending(__FUNCTION__);
void my_ending(const char* func);
void my_starting(const char* func);
#define EXIT() my_exit(__FILE__,__FUNCTION__,__LINE__);
void my_exit(const char* file, const char* func, const int line);
void my_assert(const bool condition, const char* str, const char* file, const char* func, const int line);
#define ASSERT(statement) my_assert(statement,#statement,__FILE__,__FUNCTION__,__LINE__)
template<class T> bool is_in_array(const T x, const std::vector<T>& arr) {
   for (const auto a : arr) if (a == x) return true;
   return false;
}
void print_vector(const std::vector<std::string>& vec);
template<class T> size_t index_of(const T& elem, const std::vector<T>& arr) {
   for (size_t i = 0; i < arr.size(); i++) {
      if (elem == arr[i]) return i;
   }
   ASSERT( false );
   return 0;
}

/*
   FFT
*/
void fftw_complex_to_vectors(const fftw_complex* c, const size_t N, std::vector<double>* re, std::vector<double>* im);
// bool is_in_array(const size_t x, const std::vector<size_t>& arr);
#define PRINT(var) std::cout<<#var<<" = "<<var<<std::endl;
#define ROUND(x) (x>0)?(int)(x+0.5):(int)(x-0.5)
void get_arguments(int argc, char** argv, std::string* filename, size_t* run_limit, bool* print_debug);

/* 
   CP3 
*/
#define MIN(x,y) (x<y)?x:y
#define MAX(x,y) (x>y)?x:y
// macros to conveniently grab strings from things
#define set_function_to_minimise(func) set_function_to_minimise_implementation(func,#func)
#define set_model_function(func)       set_model_function_implementation(func,#func)
// typedefs to make it more clear what I'm using the type of array for
typedef std::vector<double> Params;
typedef std::vector<double> YArray;
typedef std::vector<double> XArray;
typedef std::vector<double> EArray;
// To help readability
typedef double (*ModelFunction)(const double, const Params&); // e.g. linear/quadratic/sin/exp
typedef double (*FunctionToMinimise)(const class DataPoints&, const Params&, ModelFunction model_function); // e.g. chisq
// defined in global.cpp
XArray generate_smooth_x_values(const XArray& xinput,const size_t N_points);

/* 
   CP2
*/
// Enums for choosing which integration method and which ODE to solve
enum { EULER, RK2, RK4, ANALYTIC, METHOD_COUNT };
enum { dEdx_eq_RHO, dVdx_eq_NegE };
bool is_in_range(double min, double x, double max, double epsilon=1e-6);

#endif