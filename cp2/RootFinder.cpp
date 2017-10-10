#include <math.h>
#include <iostream>
#include <vector>
#include "RootFinder.h"

RootFinder::RootFinder(FunctionOption fo, double e) : function_option(fo), epsilon(e) {
   this->precision = -(int)log10(this->epsilon);
   switch (this->function_option) {
      case CUBIC:       this->function_name = "cubic"; break;
      case EXPONENTIAL: this->function_name = "exponential"; break;
      case SINUSOIDAL:  this->function_name = "sinusoidal"; break;
      default:          this->function_name = "unknown"; break;
   }}

double RootFinder::f(double x) { 
   switch (this->function_option) {
      case CUBIC:       return 10.2 - 7.4*x - 2.1*x*x + x*x*x;
      case EXPONENTIAL: return exp(x) - 2;
      case SINUSOIDAL:  return cos(x) * sin(3*x);
      default:          return x;
   }
}

double RootFinder::dfdx(double x) {
   double dx = 1e-7;
   double df = f(x+dx) - f(x);
   return df/dx;
}

void RootFinder::test_methods() {
   // setup and print out parameters
   double xmin = 0, xmax = 2;
   printf("x1 = %.1f, x2 = %.1f, epsilon = %.1e\n", xmin, xmax, this->epsilon);
   printf("Testing %s function...\n\n", this->function_name.c_str());

   // perform the calculations and record the iteration counts
   int n_bisect = 0, n_newton = 0, n_secant = 0, n_hybrid = 0;
   double root_bisect = bisection    (xmin, xmax, &n_bisect);
   double root_newton = newtonraphson(      xmax, &n_newton);
   double root_secant = secant       (xmin, xmax, &n_secant);
   double root_hybrid = 0.0;//hybrid       (xmin, xmax, &n_hybrid);

   // print out results
   printf("Bisection:  root = %.*f, iteration count = %d\n", this->precision+1, root_bisect, n_bisect);
   printf("Newt-Raphs: root = %.*f, iteration count = %d\n", this->precision+1, root_newton, n_newton);
   printf("Secant:     root = %.*f, iteration count = %d\n", this->precision+1, root_secant, n_secant);
   printf("Hybrid:     root = %.*f, iteration count = %d\n", this->precision+1, root_hybrid, n_hybrid);

   // print out what the results should have been
   std::vector<double> actual_roots = get_actual_roots(xmin-1, xmax+1);
   printf("Actual roots = ");
   for (auto r : actual_roots) printf("%.*f ", precision+1, r); 
   printf("\n");
}

double RootFinder::bisection(double x1, double x2, int* n) {
   double x_centre = (x2+x1)/2.0;
   double f1 = f(x1), f2 = f(x2), fc = f(x_centre);
   if      (abs(x2-x1) < this->epsilon) return x_centre;

   (*n)++; // we need to iterate, increment the counter and iterate
   if      (sign(f1) == sign(fc)) return bisection(x_centre, x2, n);
   else if (sign(f2) == sign(fc)) return bisection(x1, x_centre, n);
   else                           return 0.0; // just in case
}

double RootFinder::newtonraphson(double x, int* n) {
   double fx = f(x);
   // if f(x) is already within tolerance of 0, we've found our root
   if (abs(fx) < this->epsilon) return x;

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
   if (abs(x2-x1) < this->epsilon) return (x2+x1)/2.0;

   // y = (f2-f1)/(x2-x1)*x_centre + c = 0
   double m  = (f2-f1)/(x2-x1);
   double c  = f2 - m*x2;
   double x3 = -c / m;

   (*n)++;
   if      (x3 > x2 && x3 > x1) return secant(x2, x3, n); // x3 greater than both
   else if (x3 > x1 && x3 < x2) return secant(x1, x3, n); // x3 between the two
   else if (x3 < x1 && x3 < x2) return secant(x3, x1, n); // x3 less than both
   else                         return 0.0;               // just in case
}

double RootFinder::hybrid(double x1, double x2, int* n) {
   // if the current centre of the range is within tolerance of zero, thats the root
   double x_centre = (x2+x1)/2.0;
   if (abs(f(x_centre)) < this->epsilon) return x_centre;

   // otherwise, increment and carry on
   (*n)++;
   int n_temp = 0; // not used, just to satisfy the function arguments below
   double x3_s = secant   (x1, x2, &n_temp);
   n_temp = 0;
   double x3_b = bisection(x1, x2, &n_temp);

   // use secant version only if x3_s is within the range x1 to x2.
   // otherwise use bisection version
   double x3 = (x3_s > x2 || x3_s < x1) ? x3_b : x3_s;
   printf("x1=%f x2=%f x3_s=%f x3_b=%f x3=%f n=%d\n", x1, x2, x3_s, x3_b, x3, *n);
   if      (sign(x1) != sign(x3)) return hybrid(x1, x3, n); // x1 and x3 straddle a root
   else if (sign(x2) != sign(x3)) return hybrid(x2, x3, n); // x2 and x3 straddle a root
   else                           return 0.0;               // just in case
}

std::vector<double> RootFinder::get_actual_roots(double xmin, double xmax) {
   std::vector<double> roots = { };
   switch (this->function_option) {
      case CUBIC:
         roots = { -2.49771, 1.20296, 3.39475 };
         break;

      case EXPONENTIAL:
         roots = { log(2.0) };
         break;

      case SINUSOIDAL: {
         int n_min = (xmin < 1) ? (int)(3.0*xmin/M_PI) : (int)(xmin/M_PI + 0.5);
         for (int n = n_min; ; n++) {
            double root1 = n*M_PI/3.0;
            double root2 = M_PI*(n-0.5);
            if (is_in_range(xmin, root1, xmax)) roots.push_back(root1);
            if (is_in_range(xmin, root2, xmax)) roots.push_back(root2);

            if (!is_in_range(xmin, root1, xmax) && !is_in_range(xmin, root2, xmax))
               break;
         }
         break;
      }

      default:
         break;
   }
   return roots;
}
