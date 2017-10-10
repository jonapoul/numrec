#include <math.h>
#include <iostream>
#include "RootFinder.h"

RootFinder::RootFinder() { }

double RootFinder::f(double x) {
   return a + b*x + c*x*x + d*x*x*x;
}

double RootFinder::dfdx(double x) {
   double dx = 1e-6;
   double df = f(x+dx) - f(x);
   return df/dx;
}

void RootFinder::test_methods() {
   int counter = 0;
   int precision = (int)abs(log10(epsilon));
   double xmin = 4, xmax = 6;
   printf("Testing methods between %.1f and %.1f, with epsilon=%.1e:\n", 
          xmin, xmax, epsilon);

// 1: BISECTION
   double root_bisect = bisection(xmin, xmax, &counter);
   printf("Bisection:      root = %.*f, iteration count = %d (should be >%.2f)\n", 
          precision+1, root, counter, log2((xmax-xmin)/epsilon));
   counter = 0;
// 2: NEWTON RAPHSON
   double root_newton = newtonraphson(xmax, &counter);
   printf("Newton-Raphson: root = %.*f, iteration count = %d\n", 
          precision+1, root, counter);
   counter = 0;
// 3: SECANT
   double root_secant = secant(xmin, xmax, &counter);
   printf("Secant:         root = %.*f, iteration count = %d\n", 
          precision+1, root, counter);
   counter = 0;
// 4: HYBRID
   double root_hybrid = hybrid(xmin, xmax, &counter);
   printf("Hybrid:         root = %.*f, iteration count = %d\n", 
          precision+1, root, counter);
   counter = 0;
}

double RootFinder::bisection(double x1, double x2, int* n) {
   double x_centre = (x2+x1)/2.0;
   double f1 = f(x1), f2 = f(x2), fc = f(x_centre);
   if      (abs(x2-x1) < epsilon) return x_centre;

   (*n)++; // we need to iterate, increment the counter and iterate
   if      (sign(f1) == sign(fc)) return bisection(x_centre, x2, n);
   else if (sign(f2) == sign(fc)) return bisection(x1, x_centre, n);
   else                           return 0.0; // just in case
}

double RootFinder::newtonraphson(double x, int* n) {
   double fx = f(x);
   // if f(x) is already within tolerance of 0, we've found our root
   if (abs(fx) < epsilon) return x;

   // f(x+d) = 0 = f(x) + d*f'(x), rearrange for d
   double d = -f(x) / dfdx(x);
   (*n)++;              // we need to iterate, increment the counter
   if (*n > 100) {      // keep track of whether it's diverging
      printf("Newton-Raphson iteration count > 100, backing out...\n");
      return 0.0;
   } else {
      return newtonraphson(x+d, n);
   }
}

double RootFinder::secant(double x1, double x2, int* n) {
   double f1 = f(x1), f2 = f(x2);
   if (abs(x2-x1) < epsilon) return x1 + (x2-x1)/2.0;

   // y = (f2-f1)/(x2-x1) * x_centre + c = 0
   double m = (f2-f1)/(x2-x1);
   double c = f2 - m*x2;
   double x3 = -c / m;

   (*n)++;
   if (x3 > x2 && x3 > x1)      return secant(x2, x3, n); // x3 greater than both
   else if (x3 > x1 && x3 < x2) return secant(x1, x3, n); // x3 between the two
   else if (x3 < x1 && x3 < x2) return secant(x3, x1, n); // x3 less than both
   else                         return 0.0;                        // just in case
}

double RootFinder::hybrid(double x1, double x2, int* n) {
   // if the current centre of the range is within tolerance of zero, thats the root
   double x_centre = x1 + (x2-x1)/2.0;
   if (abs(f(x_centre)) < epsilon) return x_centre;

   // otherwise, increment and carry on
   (*n)++;
   int n_temp = 0; // not used, just to satisfy the function arguments below
   double x3_s = secant   (x1, x2, &n_temp);
   n_temp = 0;
   double x3_b = bisection(x1, x2, &n_temp);

   // use secant version only if x3_s is within the range x1 to x2
   double x3 = (x3_s > x2 || x3_s < x1) ? x3_b : x3_s;
   printf("x1=%f x2=%f x3_s=%f x3_b=%f x3=%f n=%d\n", x1, x2, x3_s, x3_b, x3, *n);
   if      (sign(x1) != sign(x3)) return hybrid(x1, x3, n); // x1 and x3 straddle a root
   else if (sign(x2) != sign(x3)) return hybrid(x2, x3, n); // x2 and x3 straddle a root
   else                           return 0.0;                        // just in case
}