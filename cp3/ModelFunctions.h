#ifndef MODELFUNCTION_H
#define MODELFUNCTION_H

#include "../global.h"

double linear     (const double x, const Params& params);   // y = a + bx
double quadratic  (const double x, const Params& params);   // y = a + bx + cx^2
double cubic      (const double x, const Params& params);   // y = a + bx + cx^2 + dx^3
double polynomial (const double x, const Params& params);   // y = a + bx + cx^2 + dx^3 + ex^4 ...
double sinusoidal (const double x, const Params& params);   // y = a + b*sin(c*x+d)
double power      (const double x, const Params& params);   // y = a + b*x^(c)
double exponential(const double x, const Params& params);   // y = a + b*c^x
double logarithmic(const double x, const Params& params);   // y = a + b*log(c*x + d)
double gaussian   (const double x, const Params& params);   // y = a + b*exp(0.5* [(x-c)/d]^2 )
double step       (const double x, const Params& params);   // y = { a if x < c } or { b for x > c }

#endif