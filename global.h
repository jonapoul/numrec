#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <vector>
#include <string>

/*
   FFT
*/
//#define round(x) ((x)>=0?(double)((x)+0.5):(double)((x)-0.5))
typedef unsigned char uchar;

/* 
   CP3 
*/
#define MIN(x,y)                       (x<y)?x:y
#define MAX(x,y)                       (x>y)?x:y
// macros to conveniently grab strings from things
#define EXIT_IF_FALSE(statement)       exit_if_false_implementation(statement,#statement,__FILE__,__FUNCTION__,__LINE__)
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
void exit_if_false_implementation(bool condition, const char* str, const char* file, const char* func, int line);
XArray generate_smooth_x_values(const XArray& xinput,const size_t N_points);

/* 
   CP2
*/
// Enums for choosing which integration method and which ODE to solve
enum { EULER, RK2, RK4, ANALYTIC, METHOD_COUNT };
enum { dEdx_eq_RHO, dVdx_eq_NegE };
bool is_in_range(double min, double x, double max, double epsilon=1e-6);

#endif