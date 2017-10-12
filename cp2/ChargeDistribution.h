#ifndef CHARGEDISTRIBUTION_H
#define CHARGEDISTRIBUTION_H

#include <vector>
#include <math.h>
#include <string>
#include "../global.h"

class ChargeDistribution {
public:
   friend class Plotter;
   ChargeDistribution();

   void generate_x_values();
   void calculate_rho_values();
   void integrate_for_E();

private:
   // integration method
   enum { EULER, RK2, RK4, ANALYTIC, METHOD_COUNT };
   // ode that we're solving for
   enum { ELECTRICFIELD, VOLTAGE };
   int current_ode; // either ELECTRICFIELD or VOLTAGE

   // specifying ranges of the charge density
   double limit0; // rho = 0         for x0<x<x1
   double limit1; // rho = +shape(x) for x1<x<x2
   double limit2; // rho = -shape(x) for x2<x<x3
   double limit3; // rho = 0         for x3<x<x4
   double limit4;
   double k;      // ?????????
   int N_points;
   double delta; // x distance between points

   array x_range;   // each x point in the range
   array rho_range; // charge density at each x point
   
   array E_euler;   // Electric field with each method
   array E_rk2;
   array E_rk4;
   array E_analytic;

   array V_euler;   // voltage with each method
   array V_rk2;
   array V_rk4;
   array V_analytic;

   double x0 = 0;  // boundary condition x
   double E0 = 0;  // boundary condition E(x0) = 0
   double V0 = 0;  // boundary condition V(x0) = 0

   double rho_shape(double x_0, double x_1, double x) const;
   double rho(double x) const;
   double E(int index, int method) const;
   double V(int index, int method) const;
   double dfdx(int index, int method) const;

   array euler() const;
   array rk2() const;
   array rk4() const;
   array analytic() const;
};

#endif