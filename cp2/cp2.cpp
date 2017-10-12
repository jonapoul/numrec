#include <iostream>
#include <string>
#include <math.h>

#include "../global.h"

// exercises
#include "RootFinder.h"
#include "ODESolver.h"
#include "Plotter.h"

// checkpoint
#include "ChargeDistribution.h"

int main() {
   Plotter plotter;

#if 0 /*Exercise 1 - Root Finding*/
   //             func   eps  xmin  xmax
   RootFinder rf(/*function*/ CUBIC, 
                 /*epsilon*/  1e-7, 
                 /*xmin*/    -2.0, 
                 /*xmax*/     2.0);
   rf.test_methods();
   plotter.plot(rf);
#endif
   
#if 0 /*Exercise 2 - ODE Solving*/
   ODESolver ode(/*function*/ ODESolver::QUADRATIC,
                 /*N*/        100, 
                 /*xmin*/    -1.0, 
                 /*xmax*/     1.0, 
                 /*x0*/       0.0, 
                 /*y(x=x0)*/  1.0);
   ode.integrate();
   plotter.plot(ode);
   plotter.plot_differences(ode);
#endif

#if 1
   // dE/dx = rho(x), 100 points, -2<x<+2, E(0)=0
   ChargeDistribution cd;
   cd.generate_x_values();
   cd.calculate_rho_values();
   cd.integrate_for_E();
   plotter.plot(cd);
#endif

   return 0;
}