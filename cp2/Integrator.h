#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "../global.h"
#include "Points.h"
#include "PNJunction.h"

class Integrator {
public:
   Integrator(PNJunction* chargedist);
   void integrate_for_E();   // use each method to find E(x), store the results in m_pn
   void integrate_for_V();   // use each method to find V(x), store the results in m_pn

private:
   PNJunction* m_pn;         // pointer to PN Junction object that we're doing the integration with
   int m_current_ode;        // enum to specify current working ODE [dE/dx=rho(x) or dV/dx=-E(x)]
   int m_current_algorithm;  // enum to specify which algorithm we're currently using. Used in dfdx(x) function

   Points euler();           // return array of x/y values of ODE solution using Euler
   Points rk2();             // return array of x/y values of ODE solution using Runge-Kutta 2nd order
   Points rk4();             // return array of x/y values of ODE solution using Runge-Kutta 4th order
   double dfdx(double x);    // calculate df/dx at position x for use in the above functions

   double single_integrate_for_E(double x);
};

#endif