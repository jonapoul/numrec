#ifndef ROOTFINDER_H
#define ROOTFINDER_H

enum FunctionOption {
   CUBIC,
   EXPONENTIAL,
   SINUSOIDAL,
   FUNCTIONCOUNT
}; 

class RootFinder {
public:
   RootFinder(FunctionOption fo, double e);

   void   test_methods ();
   double bisection    (double x1, double x2, int* n);
   double newtonraphson(double x,             int* n);
   double secant       (double x1, double x2, int* n);
   double hybrid       (double x1, double x2, int* n);

private:
   FunctionOption function_option;
   std::string function_name;
   double epsilon;
   int precision;

   double f(double x);
   double dfdx(double x);
   int sign(double x) { return (x > 0.0) ? 1 : -1; }
   bool is_in_range(double min, double x, double max) { return (min-epsilon<x && x<max+epsilon); }   
   std::vector<double> get_actual_roots(double xmin, double xmax);
};

#endif