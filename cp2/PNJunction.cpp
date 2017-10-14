#include <string>

#include "../global.h"
#include "Points.h"
#include "PNJunction.h"

PNJunction::PNJunction(int N, double* boundaries, double x_0, double E_0, double V_0) {
   this->limit0   = boundaries[0];  // xmin
   this->limit1   = boundaries[1];
   this->limit2   = boundaries[2];
   this->limit3   = boundaries[3];
   this->limit4   = boundaries[4];  // xmax
   this->N_points = N;
   this->delta    = (limit4-limit0)/(double)N;

   this->x0 = x_0;
   this->E0 = E_0;
   this->V0 = V_0;

   // allocating space for the coordinate arrays
   this->rho_points.x = std::vector<double>(N);
   this->rho_points.y = std::vector<double>(N);
   E_points = std::vector<Points>(METHOD_COUNT);
   V_points = std::vector<Points>(METHOD_COUNT);
   for (int i = 0; i < METHOD_COUNT; i++) {
      this->E_points[i].x = std::vector<double>(N);
      this->E_points[i].y = std::vector<double>(N);

      this->V_points[i].x = std::vector<double>(N);
      this->V_points[i].y = std::vector<double>(N);
   }
}

void PNJunction::initialise_points() {
   // finding where x0 fits into the range and saving that array index
   x0index = (x0-limit0)/(limit4-limit0) * N_points;

   // fill out the points
   for (int i = 0; i < N_points; i++) {
      rho_points.x[i] = x0 + (i-x0index)*delta;
      rho_points.y[i] = rho(rho_points.x[i]);
   }
   rho_points.name = "rho";
}

double PNJunction::rho_shape(double x_0, double x_1, double x) const {
   double z = (x-x_0)/(x_1-x_0);
   return pow(z,2) * (exp(1-z)-1) / 0.18;
}

double PNJunction::rho(double x) const {
   // if      (x < limit1) return  0.0;
   // else if (x < limit2) return -1.0;
   // else if (x < limit3) return  1.0;
   // else                 return  0.0;

   if      (x < limit1) return  0.0;
   else if (x < limit2) return  rho_shape(limit1, limit2, x);
   else if (x < limit3) return -rho_shape(limit3, limit2, x);
   else                 return  0.0;
}

double PNJunction::E(double XX, int algorithm) {
   Points points = E_points[algorithm];

   // find index of the x coord immediately before XX
   int closest_index = 0;
   for (size_t i = 1; i < points.size(); i++) {
      // if the current x point has switched to larger than XX, save the previous index and break
      if (XX - points.x[i] < 0) {
         closest_index = i-1;
         break;
      }
   }
   double x_0 = points.x[closest_index];
   double E_0 = points.y[closest_index];
   double dx  = XX - x_0;

   /*
      XX is an arbitrary distance dx between two discrete x values in the array.
      dx is the distance between x0 and XX, whilst delta is the same between x0 and x1.
      E_0 is the value of E at x_0, we're integrating to find E(XX), using whichever method below.
   */

   // perform a single iteration of whatever method we're using at the moment
   switch (algorithm) {
      case EULER: {
         return E_0 + dx*rho(x_0);
      } 
      case RK2: {
         double xmid = x_0 + dx/2.0;
         return E_0 + dx*rho(xmid);
      } 
      case RK4: {
         double k1 = rho(x_0);
         double k2 = rho(x_0 + dx/2.0);
         double k3 = rho(x_0 + dx/2.0);
         double k4 = rho(x_0 + dx);
         return E_0 + dx*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
      } 
      default: {
         return 0;
      }
   }
}
