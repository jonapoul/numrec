#include <math.h>
#include <iostream>

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

   // try to read command line params
   get_arguments(argc, argv);

   // calculate dx value for spacing out integrated points
   this->delta = (xmax - xmin)/(double)N_points;

   switch (option) {
      case QUADRATIC:      this->option_name = "dy/dx = 1-6x+3x^2"; break;
      case Y_X:            this->option_name = "dy/dx = y(x)";      break;
      case COS_X:          this->option_name = "dy/dx = cos(x)";    break;
      case FUNCTION_COUNT: this->option_name = "unknown";           break;
   }

   // initialising the coordinate arrays that the 3 different integration methods
   // will store results in
   this->coords = std::vector<CoordsArray>(COORDS_COUNT);
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

void ODESolver::integrate() {
   printf("Integrating %s...\n\n", this->option_name.c_str());

   // calculate all points for each method
   this->coords[EULER]  = euler();
   this->coords[RK2]    = rk2();
   this->coords[RK4]    = rk4();
   this->coords[ACTUAL] = analytic();

   // calculate the differences between each method and the actual curve
   this->differences = calculate_differences();

}

CoordsArray ODESolver::euler() {
   CoordsArray output;
   output.name = "Euler";

   // these are to allow for x0 to be outside the range xmin->xmax
   double xmin_local = MIN(x0, xmin);
   double xmax_local = MAX(x0, xmax);

   // get all x values in the (possibly extended) range
   for (int i = 0; /*blank*/ ; i++) {
      // if this value is within the range we want, add it to the array
      double x_temp = xmin_local + delta*i;
      if (x_temp <= xmax_local) {
         output.x.push_back(x_temp);
         output.y.push_back(0.0);
      } else break;
      
      // if this is close to the x boundary condition, save the index for future reference
      if (abs(output.x[i] - x0) < delta/2.0) 
         this->x0_index = i;
   }
   // shifting all x values so that the nearest one to x0 is exactly x0
   double xshift = x0 - output.x[x0_index];
   for (int i = 0; i < (int)output.x.size(); i++) {
      output.x[i] += xshift;
   }

   // calculate y values from x0 up to xmax
   output.y[this->x0_index] = this->y0;
   this->x_size = (int)output.x.size();
   for (int i = this->x0_index+1; i < x_size && output.x[i] <= this->xmax; i++) {
      output.y[i] = output.y[i-1] + this->delta * dfdx(output.x[i-1], output.y[i-1]);
   }
   // same from x0 down to xmin
   for (int i = this->x0_index-1; i >= 0; i--) {
      output.y[i] = output.y[i+1] - this->delta * dfdx(output.x[i+1], output.y[i+1]);
   }
   printf("Euler xsize=%d ysize=%d\n", (int)output.x.size(), (int)output.y.size());
   return output; 
}

CoordsArray ODESolver::rk2() const {
   CoordsArray output(this->x_size);
   output.x = this->coords[EULER].x; // copy across precalculated range of x
   output.name = "RK2";

   return output;
}

CoordsArray ODESolver::rk4() const {
   CoordsArray output(this->x_size);
   output.x = this->coords[EULER].x; // copy across precalculated range of x
   output.name = "RK4";

   return output;
}

CoordsArray ODESolver::analytic() const {
   CoordsArray output(this->x_size);
   output.x = this->coords[EULER].x; // copy across precalculated range of x
   output.name = "Analytic Solution";
   for (int i = 0; i < this->x_size; i++) {
      output.y[i] = f(output.x[i]);
   }
   return output;
}

std::vector<CoordsArray> ODESolver::calculate_differences() const {
   std::vector<CoordsArray> diffs(ACTUAL);
   for (int i = 0; i < ACTUAL; i++) {
      // initialise each array
      diffs[i].x    = this->coords[EULER].x; // copy across precalculated range of x
      diffs[i].y    = array(this->x_size);
      diffs[i].name = this->coords[i].name;

      // fill each y array with the differences
      for (int j = 0; j < this->x_size; j++) {
         diffs[i].y[j] = coords[ACTUAL].y[j] - coords[i].y[j];
      }
   }
   return diffs;
}

void ODESolver::get_arguments(int argc, char** argv) {
   std::string name;
   std::string valuestr;
   for (int i = 1; i < argc; i++) {
      // splits argv[i] into parameter name and its value as a string
      parse(argv[i], &name, &valuestr);

      // check whether the input name is specified, and apply its value
      if (name == "func") {
         int temp = std::stoi(valuestr);
         if (0 <= temp && temp <= FUNCTION_COUNT)
            this->option = static_cast<ODEFunction>(temp);
      } else if (name == "N") {
         this->N_points = std::stoi(valuestr);
      } else if (name == "xmin") {
         this->xmin = std::stod(valuestr);
      } else if (name == "xmax") {
         this->xmax = std::stod(valuestr);
      } else if (name == "x0") {
         this->x0 = std::stod(valuestr);
      } else if (name == "y0") {
         this->y0 = std::stod(valuestr);
      }
   }
}