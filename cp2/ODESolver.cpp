#include <math.h>
#include "ODESolver.h"

ODESolver::ODESolver(int argc, char** argv) {
   // defaults
   this->function_option = QUADRATIC;
   this->xmin = -3;
   this->xmax =  3;

   // trying to read command line params
   if (argc > 1) { // function option
      int temp = std::stoi(argv[1]);
      if (0 <= temp && temp <= ODE_FUNCTION_COUNT)
         this->function_option = static_cast<ODEFunction>(temp);
   }
   if (argc > 3) {
      this->xmin = std::stod(argv[2]);
      this->xmax = std::stod(argv[3]);
   }

   switch (function_option) {
      case QUADRATIC:          this->function_name = "dy/dx = 1-6x+3x^2"; break;
      case Y_X:                this->function_name = "dy/dx = y(x)";      break;
      case COS_X:              this->function_name = "dy/dx = cos(x)";    break;
      case ODE_FUNCTION_COUNT: this->function_name = "unknown";           break;
   }
}

double ODESolver::dfdx(double x) const {
   switch (this->function_option) {
      case QUADRATIC: return 1 - 6*x * 3*x*x;
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
