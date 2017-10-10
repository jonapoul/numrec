#ifndef ROOTFINDER_H
#define ROOTFINDER_H

#include <vector>

#include "../global.h"

enum RootFunction {
   CUBIC,
   EXPONENTIAL,
   SINUSOIDAL,
   ROOT_FUNCTION_COUNT
}; 

class RootFinder {
public:
   friend class Plotter;

   RootFinder(int argc, char** argv);

   void   test_methods ()                             const;
   void   plot_function()                             const;
   double bisection    (double x1, double x2, int* n) const;
   double newtonraphson(double x,             int* n) const;
   double secant       (double x1, double x2, int* n) const;
   double hybrid       (double x1, double x2, int* n) const;

private:
   RootFunction function_option;
   std::string function_name;
   double epsilon;
   int precision;
   double xmin;
   double xmax;

   double f(const double x) const;
   double dfdx(const double x) const;
   int    sign(const double x) const { return (x > 0.0) ? 1 : -1; }

   bool   is_in_range(double min, double x, double max) const { return (min-epsilon<x && x<max+epsilon); }   
   array  get_actual_roots(double xmin, double xmax) const;
};

#endif