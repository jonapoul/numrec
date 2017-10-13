#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "../global.h"
#include "Points.h"
#include "ChargeDistribution.h"

class Integrator {
public:
   Integrator(ChargeDistribution* chargedist);
   void integrate_for_E();
   void integrate_for_V();

private:
   ChargeDistribution* cd;
   double xmax;
   double xmin;
   double delta;

   Points euler(int ode);
   Points rk2(int ode);
   Points rk4(int ode);
   double dfdx(double x, int ode);
};

#endif