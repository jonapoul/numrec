#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "../global.h"
#include "RootFinder.h"

RootFinder::RootFinder(int argc, char** argv) {
   // default values
   this->function_option = CUBIC;
   this->epsilon = 1e-7;
   this->xmin = 0.0;
   this->xmax = 2.0;

   // tries to read input parameters
   if (argc > 1) { // function option
      int temp = std::stoi(argv[1]);
      if (0 <= temp && temp < ROOT_FUNCTION_COUNT) {
         this->function_option = static_cast<RootFunction>(temp);
      } else {
         printf("You can't put %d as the function choice. Pick one from %d-%d\n",
                temp, 0, ROOT_FUNCTION_COUNT-1);
         exit(1);
      }
   }
   if (argc > 2) { // epsilon power
      this->epsilon = std::stod(argv[2]);
   }
   if (argc > 4) { // xmin/max
      this->xmin = std::stod(argv[3]);
      this->xmax = std::stod(argv[4]);
   }

   // used to format printf
   this->precision = -static_cast<int>(log10(this->epsilon));
   // 
   switch (this->function_option) {
      case CUBIC:       this->function_name = "cubic";       break;
      case EXPONENTIAL: this->function_name = "exponential"; break;
      case SINUSOIDAL:  this->function_name = "sinusoidal";  break;
      default:          this->function_name = "unknown";     break;
   }
}

double RootFinder::f(double x) const { 
   switch (this->function_option) {
      case CUBIC:       return 10.2 - 7.4*x - 2.1*x*x + x*x*x;
      case EXPONENTIAL: return exp(x) - 2;
      case SINUSOIDAL:  return cos(x) * sin(3*x);
      default:          return 0.0;
   }
}

double RootFinder::dfdx(double x) const {
   double dx = 1e-7;
   double df = f(x+dx) - f(x);
   return df/dx;
}

void RootFinder::test_methods() const {
   // setup and print out parameters
   printf("x1 = %.1f, x2 = %.1f, epsilon = %.1e\n", this->xmin, this->xmax, this->epsilon);
   printf("Testing %s function...\n\n", this->function_name.c_str());

   // perform the calculations and record the iteration counts
   int n_bisect = 0, n_newton = 0, n_secant = 0, n_hybrid = 0;
   double root_bisect = bisection    (this->xmin, this->xmax, &n_bisect);
   double root_newton = newtonraphson(            this->xmax, &n_newton);
   double root_secant = secant       (this->xmin, this->xmax, &n_secant);
   double root_hybrid = hybrid       (this->xmin, this->xmax, &n_hybrid);

   // print out results
   printf("Bisection:  root = %.*f, iteration count = %d\n", this->precision+1, root_bisect, n_bisect);
   printf("Newt-Raphs: root = %.*f, iteration count = %d\n", this->precision+1, root_newton, n_newton);
   printf("Secant:     root = %.*f, iteration count = %d\n", this->precision+1, root_secant, n_secant);
   printf("Hybrid:     root = %.*f, iteration count = %d\n", this->precision+1, root_hybrid, n_hybrid);

   // print out what the results should have been
   // the range passed to get_actual_results is slightly wider to show edge roots
   array actual_roots = get_actual_roots(this->xmin-1, this->xmax+1);
   printf("Actual roots = \n");
   for (auto r : actual_roots) 
      printf("\t%.*f\n", this->precision+1, r); 
}

double RootFinder::bisection(double x1, double x2, int* n) const {
   double x_centre = (x2+x1)/2.0;
   double f1 = f(x1), f2 = f(x2), fc = f(x_centre);
   if      (abs(x2-x1) < this->epsilon) return x_centre;

   (*n)++; // we need to iterate, increment the counter and iterate
   if      (sign(f1) == sign(fc)) return bisection(x_centre, x2, n);
   else if (sign(f2) == sign(fc)) return bisection(x1, x_centre, n);
   else                           return 0.0; // just in case
}

double RootFinder::newtonraphson(double x, int* n) const {
   double fx = f(x);
   // if f(x) is already within tolerance of 0, we've found our root
   if (abs(fx) < this->epsilon) return x;

   // f(x+d) = 0 = f(x) + d*f'(x), rearrange for d
   double d = -f(x) / dfdx(x);
   (*n)++;              // we need to iterate, increment the counter
   if (*n > 100) {      // keep track of whether it's diverging
      printf("Newton-Raphson iteration count > 100, quitting...\n");
      return 0.0;
   } else {
      return newtonraphson(x+d, n);
   }
}

double RootFinder::secant(double x1, double x2, int* n) const {
   double f1 = f(x1), f2 = f(x2);
   if (abs(x2-x1) < this->epsilon) return (x2+x1)/2.0;

   // y = m*x3 + c = 0
   double m  = (f2-f1)/(x2-x1);
   double c  = f2 - m*x2;
   double x3 = -c / m;

   (*n)++; // increment the iteration counter
   if      (x3 > x2 && x3 > x1) return secant(x2, x3, n); // x3 greater than both
   else if (x3 > x1 && x3 < x2) return secant(x1, x3, n); // x3 between the two
   else if (x3 < x1 && x3 < x2) return secant(x3, x1, n); // x3 less than both
   else                         return 0.0;               // just in case
}

double RootFinder::hybrid(double x1, double x2, int* n) const {
   if (*n > 100) {
      printf("Hybrid iteration count > 1000, quitting...\n");
      return 0.0;
   }

   // calculate next x value via secant method
   double f2 = f(x2), f1 = f(x1);
   double m = (f2-f1)/(x2-x1);
   double c = f2 - m*x2;
   double x3_s = -c / m;
   // calculate next x value via bisection method
   double x3_b = (x1 + x2) / 2.0;

   // use secant version only if x3_s is within the range x1 to x2.
   // otherwise use bisection version
   double x3 = (x3_s > x2 || x3_s < x1) ? x3_b : x3_s;
   double f3 = f(x3);
   // if x3 is close enough to zero, that's our root
   if (abs(f3) < this->epsilon) return x3;

   // otherwise, increment and iterate again
   (*n)++;
   if      (sign(f1) != sign(f3)) return hybrid(x1, x3, n); // x1 and x3 straddle a root
   else if (sign(f2) != sign(f3)) return hybrid(x2, x3, n); // x2 and x3 straddle a root
   else                           return 0.0;               // just in case
}

array RootFinder::get_actual_roots(double x_min, double x_max) const {
   array roots = { };
   switch (this->function_option) {
      case CUBIC:
         roots = { -2.49771, 1.20296, 3.39475 };
         break;

      case EXPONENTIAL:
         roots = { log(2.0) };
         break;

      case SINUSOIDAL: {
         // nmin is the minimum value of n for which there would be a root in the range
         int n_min = (x_min < 1) ? (int)(3.0*x_min/M_PI) : (int)(x_min/M_PI + 0.5);
         for (int n = n_min; ; n++) {
            double root1 = n*M_PI/3.0;
            double root2 = M_PI*(n-0.5);
            if (is_in_range(x_min, root1, x_max)) roots.push_back(root1);
            if (is_in_range(x_min, root2, x_max)) roots.push_back(root2);

            // if neither are in the range, back out of the loop and return
            if (!is_in_range(x_min, root1, x_max) && 
                !is_in_range(x_min, root2, x_max) && 
                n-n_min > 4)
               break;
         }
         break;
      }

      default:
         break;
   }
   // sort into order and return
   std::sort(roots.begin(), roots.end(), [](double a, double b) { return a < b; });
   return roots;
}
