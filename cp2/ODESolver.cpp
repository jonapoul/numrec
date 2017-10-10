#include <math.h>

#include "../global.h"
#include "Plotter.h"
#include "ODESolver.h"

ODESolver::ODESolver(int argc, char** argv) {
   // defaults
   this->function_option = QUADRATIC;
   this->N_points = 1e4;
   this->xmin = -3;
   this->xmax =  3;
   this->x0 = 0;
   this->y0 = 1;

   // trying to read command line params
   if (argc > 1) { // function option
      int temp = std::stoi(argv[1]);
      if (0 <= temp && temp <= ODE_FUNCTION_COUNT)
         this->function_option = static_cast<ODEFunction>(temp);
   }
   if (argc > 2) {
      this->N_points = std::stoi(argv[2]);
   }
   if (argc > 4) {
      this->xmin = std::stod(argv[3]);
      this->xmax = std::stod(argv[4]);
   }
   this->delta = (xmax - xmin)/(double)N_points;

   switch (function_option) {
      case QUADRATIC:          this->function_name = "dy/dx = 1-6x+3x^2"; break;
      case Y_X:                this->function_name = "dy/dx = y(x)";      break;
      case COS_X:              this->function_name = "dy/dx = cos(x)";    break;
      case ODE_FUNCTION_COUNT: this->function_name = "unknown";           break;
   }
}

double ODESolver::dfdx(double x) const {
   switch (this->function_option) {
      case QUADRATIC: return 1 - 6*x + 3*x*x;
      case Y_X:       return f(x);
      case COS_X:     return cos(x);
      default:        return 0.0;
   }
}

double ODESolver::f(double x) const {
   switch (this->function_option) {
      case QUADRATIC: return 0.0;
      case Y_X:       return 0.0;
      case COS_X:     return 0.0;
      default:        return 0.0;
   }
}

void ODESolver::test_methods() const {
   // setup and print out parameters
   printf("x0 = %.2f, y(x0) = %.2f, xmin = %.1f, xmax = %.1f\n", this->x0, this->y0, this->xmin, this->xmax);
   printf("Testing %s function...\n\n", this->function_name.c_str());

   array x(this->N_points), y(this->N_points);
   euler(this->x0, this->y0, &x, &y);
   plot(x, y);
}

void ODESolver::euler(double x0, double y0, array* x, array* y) const {
/*
      NEED TO BE ABLE TO EXTRAPOLATE BACKWARDS FROM X0, NOT JUST FORWARDS
*/
   (*x)[0] = this->x0;
   (*y)[0] = this->y0;
   for (int i = 1; i < this->N_points; i++) {
      (*x)[i] = (*x)[i-1] + this->delta;
      (*y)[i] = (*y)[i-1] + this->delta * dfdx((*x)[i]);
   }
}
