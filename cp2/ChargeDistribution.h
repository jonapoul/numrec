#ifndef CHARGEDISTRIBUTION_H
#define CHARGEDISTRIBUTION_H

#include <vector>
#include <math.h>
#include <string>
#include "../global.h"
#include "Points.h"

class ChargeDistribution {
public:
   friend class Plotter;
   friend class Integrator;
   ChargeDistribution(int N, double* boundaries, double x_0, double E_0, double V_0);

   void initialise_points();
   double rho(double x) const;

   Points rho_points; 
   std::vector<Points> E_points;
   std::vector<Points> V_points;

private:
   // specifying ranges of the charge density
   double limit0; // rho = 0         for x0<x<x1
   double limit1; // rho = +shape(x) for x1<x<x2
   double limit2; // rho = -shape(x) for x2<x<x3
   double limit3; // rho = 0         for x3<x<x4
   double limit4;
   int N_points;
   double delta; // x distance between points

   double x0;   // boundary condition x
   int x0index; // index of x0 in the points.x arrays
   double E0;   // boundary condition E(x0) = 0
   double V0;   // boundary condition V(x0) = 0

   double rho_shape(double x_0, double x_1, double x) const;
};

#endif