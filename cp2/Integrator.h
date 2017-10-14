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
   PNJunction* pn;
   double delta;
   int current_ode;
   int current_algorithm;

   Points euler();
   Points rk2();
   Points rk4();
   double dfdx(double x);
};

#endif