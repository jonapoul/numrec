#include <math.h>
#include <iostream>
#include <algorithm>

#include "../global.h"
#include "Plotter.h"
#include "ODESolver.h"

ODESolver::ODESolver(ODEFunction f, int N, double min, double max, double x_0, double y_0) {
   this->option = f;
   this->N_points = N;
   this->xmin = min;
   this->xmax = max;
   this->x0 = x_0;
   this->y0 = y_0;
   this->delta = (max - min)/(double)N;

   switch (f) {
      case QUADRATIC: this->option_name = "dy/dx = 1-6x+3x^2"; break;
      case Y_X:       this->option_name = "dy/dx = y(x)";      break;
      case COS_X:     this->option_name = "dy/dx = cos(x)";    break;
      default:        this->option_name = "unknown";           break;
   }

   // setup coordinate arrays that the 3 different integration methods will store results in
   this->coords = std::vector<CoordsArray>(METHOD_COUNT);
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

   // array of all x points that we need
   this->x_range = get_x_values();

   // calculate all points for each method
   this->coords[EULER]    = euler();
   this->coords[RK2]      = rk2();
   this->coords[RK4]      = rk4();
   this->coords[ANALYTIC] = analytic();

   // calculate the differences between each method and the actual curve
   this->differences = calculate_differences();
}

CoordsArray ODESolver::euler(const array& x_values) const {
   CoordsArray output;
   output.name = "Euler";
   // if the user specifies a range of x values to use, take that instead
   output.x = (x_values.size() > 0) ? x_values : this->x_range;
   output.y = array(this->x_size);
   output.y[this->x0_index] = y0;

   // calculate y values from x0 up to xmax
   for (int i = this->x0_index+1; i < x_size && output.x[i] <= this->xmax; i++) {
      output.y[i] = output.y[i-1] + this->delta * dfdx(output.x[i-1], output.y[i-1]);
   }
   // same from x0 down to xmin
   for (int i = this->x0_index-1; i >= 0; i--) {
      output.y[i] = output.y[i+1] - this->delta * dfdx(output.x[i+1], output.y[i+1]);
   }
   return output; 
}

CoordsArray ODESolver::rk2(const array& x_values) const {
   CoordsArray output;
   output.name = "RK2";
   output.x = (x_values.size() > 0) ? x_values : this->x_range;
   output.y = array(this->x_size);
   output.y[x0_index] = y0;

   // calculate y values from x0 up to xmax
   for (int i = this->x0_index+1; i < this->x_size; i++) {
      double xmid = output.x[i-1] + this->delta/2.0;
      double ymid = output.y[i-1] + dfdx(output.x[i-1], output.y[i-1])*(this->delta/2.0);
      output.y[i] = output.y[i-1] + dfdx(xmid, ymid)*delta;
   }
   // same from x0 down to xmin
   for (int i = this->x0_index-1; i >= 0; i--) {
      double xmid = output.x[i+1] - this->delta/2.0;
      double ymid = output.y[i+1] - dfdx(output.x[i+1], output.y[i+1])*(this->delta/2.0);
      output.y[i] = output.y[i+1] - dfdx(xmid, ymid)*(this->delta);
   }

   return output;
}

CoordsArray ODESolver::rk4(const array& x_values) const {
   CoordsArray output;
   output.name = "RK4";
   output.x = (x_values.size() > 0) ? x_values : this->x_range;
   output.y = array(this->x_size);
   output.y[x0_index] = y0;

   // calculate y values from x0 up to xmax
   for (int i = this->x0_index+1; i < this->x_size; i++) {
      double x_prev = output.x[i-1];
      double y_prev = output.y[i-1];
      double k1 = dfdx(x_prev,             y_prev);
      double k2 = dfdx(x_prev + delta/2.0, y_prev + k1*delta/2.0);
      double k3 = dfdx(x_prev + delta/2.0, y_prev + k2*delta/2.0);
      double k4 = dfdx(x_prev + delta,     y_prev + k3*delta);
      output.y[i] = y_prev + delta*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
   }

   // same from x0 down to xmin
   for (int i = this->x0_index-1; i >= 0; i--) {
      double x_prev = output.x[i+1];
      double y_prev = output.y[i+1];
      double k1 = dfdx(x_prev,             y_prev);
      double k2 = dfdx(x_prev - delta/2.0, y_prev - k1*delta/2.0);
      double k3 = dfdx(x_prev - delta/2.0, y_prev - k2*delta/2.0);
      double k4 = dfdx(x_prev - delta,     y_prev - k3*delta);
      output.y[i] = y_prev - delta*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
   }

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
   std::vector<CoordsArray> diffs(METHOD_COUNT-1);
   for (int i = 0; i < ANALYTIC; i++) {
      // initialise each array
      diffs[i].x    = this->coords[EULER].x; // copy across precalculated range of x
      diffs[i].y    = array(this->x_size);
      diffs[i].name = this->coords[i].name;

      // fill each y array with the differences
      for (int j = 0; j < this->x_size; j++) {
         diffs[i].y[j] = coords[ANALYTIC].y[j] - coords[i].y[j];
         if (i == EULER)
            diffs[i].y[j] /= 20.0;
      }
   }
   return diffs;
}

array ODESolver::get_x_values() {
   array x;
   // these are to allow for x0 to be outside the range xmin->xmax
   double xmin_local = MIN(x0, xmin);
   double xmax_local = MAX(x0, xmax);

   // get all x values in the (possibly extended) range
   for (int i = 0;  ; i++) {
      // if this value is within the range we want, add it to the array
      double x_temp = xmin_local + delta*i;
      if (x_temp <= xmax_local) {
         x.push_back(x_temp);
      } else {
         break;
      }
      // if this is close to the x boundary condition, save the index for future reference
      if (abs(x[i] - x0) < delta/2.0) {
         this->x0_index = i;
      }
   }
   // shifting all x values so that the nearest one to x0 is exactly x0
   double xshift = x0 - x[x0_index];
   for (int i = 0; i < (int)x.size(); i++) {
      x[i] += xshift;
   }
   this->x_size = (int)x.size();

   std::sort(x.begin(), x.end(), [](const double& a, const double& b) { return a < b; });
   return x;
}