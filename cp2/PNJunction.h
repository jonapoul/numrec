#ifndef CHARGEDISTRIBUTION_H
#define CHARGEDISTRIBUTION_H

#include <vector>
#include <math.h>
#include <string>
#include "../global.h"
#include "Points.h"

class PNJunction {
public:
   friend class Plotter;
   friend class Integrator;
   
   PNJunction(int N, double* boundaries, double x_0, double E_0, double V_0);
   void initialise_points();
   double E(double x, int algorithm);

   Points rho_points;            // exact values of rho(x)
   std::vector<Points> E_points; // coords of each E(x) value, for each integration method
   std::vector<Points> V_points; // coords of each V(x) value, for each integration method

private:
   // specifying ranges of the charge density
   double limit0; // rho = 0         for limit0 < x < limit1
   double limit1; // rho = +shape(x) for limit1 < x < limit2
   double limit2; // rho = -shape(x) for limit2 < x < limit3
   double limit3; // rho = 0         for limit3 < x < limit4
   double limit4;
   int N_points;
   double delta; // x distance between points

   double x0;   // boundary condition x
   int x0index; // index of x0 in the points.x arrays
   double E0;   // boundary condition E(x0) = 0
   double V0;   // boundary condition V(x0) = 0

   double rho(double x) const;
   double rho_shape(double x_0, double x_1, double x) const;
};

#endif