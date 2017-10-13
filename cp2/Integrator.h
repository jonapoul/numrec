#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "../global.h"
#include "Points.h"
#include "PNJunction.h"

class Integrator {
public:
   Integrator(PNJunction* chargedist);
   void integrate_for_E();
   void integrate_for_V();

private:
   PNJunction* cd;
   double xmax;
   double xmin;
   double delta;

   Points euler(int ode);
   Points rk2(int ode);
   Points rk4(int ode);
   double dfdx(double x, int ode);
};

#endif