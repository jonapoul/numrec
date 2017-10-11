#include <math.h>

#include "../global.h"
#include "Plotter.h"
#include "ODESolver.h"

ODESolver::ODESolver(int argc, char** argv) {
   // defaults
   this->option = QUADRATIC;
   this->N_points = 1e4;
   this->xmin = -3;
   this->xmax =  3;
   this->x0 = 0;
   this->y0 = 1;

   // trying to read command line params
   if (argc > 1) { // function option
      int temp = std::stoi(argv[1]);
      if (0 <= temp && temp <= ODE_FUNCTION_COUNT)
         this->option = static_cast<ODEFunction>(temp);
   }
   if (argc > 2) {
      this->N_points = std::stoi(argv[2]);
   }
   if (argc > 4) {
      this->xmin = std::stod(argv[3]);
      this->xmax = std::stod(argv[4]);
   }

   // calculate dx value for spacing out integrated points
   this->delta = (xmax - xmin)/(double)N_points;

   switch (option) {
      case QUADRATIC:          this->option_name = "dy/dx = 1-6x+3x^2"; break;
      case Y_X:                this->option_name = "dy/dx = y(x)";      break;
      case COS_X:              this->option_name = "dy/dx = cos(x)";    break;
      case ODE_FUNCTION_COUNT: this->option_name = "unknown";           break;
   }

   // initialising the coordinate arrays that the 3 different integration methods
   // will store results in
   this->coords = std::vector<CoordsArray>(ODE_COORDS_COUNT);
}

double ODESolver::dfdx(double x, double y) const {
   switch (this->option) {
      case QUADRATIC: return 1 - 6*x + 3*x*x;
      case Y_X:       return y;
      case COS_X:     return cos(x);
      default:        return 0.0;
   }
}

double ODESolver::f(double x) const {
   double c;
   switch (this->option) {
      case QUADRATIC:
         c = y0 - x0 + 3*x0*x0 - x0*x0*x0;
         return x - 3*x*x + x*x*x + c;
      case Y_X:
         c = y0 / exp(x0);
         return c*exp(x);
      case COS_X:
         c = y0 - sin(x0);
         return sin(x) + c;
      default:
         return 0.0;
   }
}

void ODESolver::test_methods() {
   // setup and print out parameters
   printf("x0 = %.2f, y(x0) = %.2f, xmin = %.1f, xmax = %.1f\n", 
          this->x0, this->y0, this->xmin, this->xmax);
   printf("Testing %s function...\n\n", this->option_name.c_str());

   this->coords[EULER]  = euler();
   this->coords[RK2]    = rk2();
   this->coords[RK4]    = rk4();
   this->coords[ACTUAL] = actual();

   this->differences = calculate_differences();
}

CoordsArray ODESolver::euler() const {
/*
      NEED TO BE ABLE TO EXTRAPOLATE BACKWARDS FROM X0, NOT JUST FORWARDS
*/
   CoordsArray coords(this->N_points);
   coords.name = "Euler";
   coords.x[0] = this->x0;
   coords.y[0] = this->y0;
   for (int i = 1; i < this->N_points; i++) {
      coords.x[i] = coords.x[i-1] + this->delta;
      coords.y[i] = coords.y[i-1] + this->delta * dfdx(coords.x[i-1], coords.y[i-1]);
   }
   return coords;
}

CoordsArray ODESolver::rk2() const {
   CoordsArray coords(this->N_points);
   coords.name = "RK2";

   return coords;
}

CoordsArray ODESolver::rk4() const {
   CoordsArray coords(this->N_points);
   coords.name = "RK4";

   return coords;
}

CoordsArray ODESolver::actual() const {
   CoordsArray coords(this->N_points);
   coords.name = "Actual";

   coords.x[0] = this->xmin;
   coords.y[0] = f(coords.x[0]);
   for (int i = 1; i < this->N_points; i++) {
      coords.x[i] = coords.x[i-1] + this->delta;
      coords.y[i] = f(coords.x[i]);
   }
   return coords;
}

std::vector<CoordsArray> ODESolver::calculate_differences() const {
   std::vector<CoordsArray> diffs(ODE_COORDS_COUNT-1);
   for (int i = 0; i < ODE_COORDS_COUNT-1; i++) {
      diffs[i].x    = array(this->N_points);
      diffs[i].y    = array(this->N_points);
      diffs[i].name = this->coords[i].name;
      for (int j = 0; j < this->N_points; j++) {
         diffs[i].x[j] = this->xmin + j*this->delta;
         diffs[i].y[j] = coords[ACTUAL].y[j] - coords[i].y[j];
      }
   }
   return diffs;
}