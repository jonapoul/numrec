#include <string>

#include "../global.h"
#include "Points.h"
#include "ChargeDistribution.h"

ChargeDistribution::ChargeDistribution(int N, double* boundaries, double x_0, double E_0, double V_0) {
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

   this->rho_points = Points(N_points, "rho(x)");
   this->E_points   = {
      Points(N_points, "E_euler"),
      Points(N_points, "E_rk2"),
      Points(N_points, "E_rk4"),
   };
   this->V_points   = {
      Points(N_points, "V_euler"),
      Points(N_points, "V_rk2"),
      Points(N_points, "V_rk4"),
   };
}

void ChargeDistribution::initialise_points() {
   // finding where x0 fits into the range and saving that array index
   x0index = (x0-limit0)/(limit4-limit0) * N_points;

   // fill out the points
   for (int i = 0; i < N_points; i++) {
      rho_points.x[i] = x0 + (i-x0index)*delta;
      rho_points.y[i] = rho(rho_points.x[i]);
   }
}

double ChargeDistribution::rho_shape(double x_0, double x_1, double x) const {
   double z = (x-x_0)/(x_1-x_0);
   return pow(z,2) * (exp(1-z)-1) / 0.18;
}

double ChargeDistribution::rho(double x) const {
   if      (x < limit1) return  0.0;
   else if (x < limit2) return  rho_shape(limit1, limit2, x);
   else if (x < limit3) return -rho_shape(limit3, limit2, x);
   else /*if (x < x4)*/ return  0.0;
}
