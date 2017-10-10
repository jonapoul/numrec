#include <string>
#include "ChargeDistribution.h"

ChargeDistribution::ChargeDistribution() {
   x0 = -2;
   x1 = -1;
   x2 =  0;
   x3 =  1;
   x4 =  2;
   k  = M_PI/(x3-x1);
}

double ChargeDistribution::shape(double x0, double x1, double x) const {
   auto z = (x-x0)/(x1-x0);
   return pow(z, 2) * (exp(1-z)-1) / 0.18;
}

double ChargeDistribution::evaluate(double x) const {
   if      (x < this->x1) return  0;
   else if (x < this->x2) return  shape(this->x1, this->x2, x);
   else if (x < this->x3) return -shape(this->x3, this->x2, x);
   else                   return  0;
}

void ChargeDistribution::get_XY_values(double start,
                                       double end,
                                       int N_points,
                                       /*output*/
                                       vector<double>* x, 
                                       vector<double>* y) const {
   // initialise both to arrays of zeroes
   *x = std::vector<double>(N_points, 0.0);
   *y = std::vector<double>(N_points, 0.0);
   double dx = (end - start) / static_cast<double>(N_points);

   // fill arrays with function values
   for (int i = 0; i < N_points; i++) {
      (*x)[i] = start + i*dx;
      (*y)[i] = evaluate(start + i*dx);
   }
}