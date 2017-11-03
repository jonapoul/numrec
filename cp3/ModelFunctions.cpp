#include <math.h>
#include "../global.h"
#include "ModelFunctions.h"

// y = a + bx
double linear(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 2 );
   const double a = params[0];
   const double b = params[1];
   return a + b*x;
}

// y = a + bx^2
double quadratic(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 3 );
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   return a + b*x + c*x*x;
}

// y = a + bx^2 + cx^3
double cubic(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 4 );
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   const double d = params[3];
   return a + b*x + c*x*x + d*x*x*x;
}

// generalisation of linear/quad/cubic for arbitrary number of parameters (at least 1)
double polynomial(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() >= 1 );
   double sum = 0.0;
   for (size_t i = 0; i < params.size(); i++) {
      sum += params[i] * pow(x, i);
   }
   return sum;
}

// y = a + b*sin(c*x+d)
double sinusoidal(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 4 );
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   const double d = params[3];
   return a + b*sin(c*x + d);
}

// y = a + b*x^(c)
double power(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 3 );
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   return a + b*pow(x, c);
}

// y = a + b*c^x
double exponential(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 4 );
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   const double d = params[3];
   return a + b*pow(c, d*x);
}

// y = a + b*log(c*x + d)
double logarithmic(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 4 );
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   const double d = params[3];
   return a + b*log(c*x + d);
}

// y = a + b*exp(0.5* [(x-c)/d]^2 )
double gaussian(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 4 );
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   const double d = params[3];
   return a + b*exp( -0.5 * pow((x-c)/d, 2.0) );
}

// y = { a  for x < c } or { b  for x > c }
double step(const double x, const Params& params) {
   EXIT_IF_FALSE( params.size() == 3 );
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   return (x < c) ? a : b;
}