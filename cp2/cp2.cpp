#include <iostream>
#include <string>
#include <math.h>

#include "../global.h"

// exercises
#include "RootFinder.h"
#include "ODESolver.h"

// checkpoint
#include "ChargeDistribution.h"
#include "Integrator.h"
#include "Plotter.h"

#define CURRENT 3

int main() {
#if (CURRENT == 1) /*Exercise 1 - Root Finding*/
   //             func   eps  xmin  xmax
   RootFinder rf(/*function*/ CUBIC, 
                 /*epsilon*/  1e-7, 
                 /*xmin*/    -2.0, 
                 /*xmax*/     2.0);
   rf.test_methods();
   Plotter plotter;
   plotter.plot(rf);
#endif

#if (CURRENT == 2) /*Exercise 2 - ODE Solving*/
   ODESolver ode(/*function*/ COS_X,
                 /*N*/        100, 
                 /*xmin*/     0.0, 
                 /*xmax*/     10.0, 
                 /*x0*/       0.0, 
                 /*y(x=x0)*/  0.0);
   ode.integrate();
   Plotter plotter;
   plotter.plot(ode);
   plotter.plot_differences(ode);
#endif

#if (CURRENT == 3) /*Checkpoint 2 - PN Junctions*/

   int N_points = 1000;                                  // number of points
   double x_boundaries[5] = {-2.0, -1.0, 0.0, 1.0, 2.0}; // limits of rho(x)
   
   // integration boundary conditions
   double x0 = -2.0;
   double E0 =  0.0;
   double V0 =  0.0;

   ChargeDistribution cd(N_points, x_boundaries, x0, E0, V0);
   cd.initialise_points(); // calculating values of rho(x) at each point

   Integrator ig(&cd);
   ig.integrate_for_E();   // use each of the 3 methods to solve dE/dx = rho(x)
   ig.integrate_for_V();   // same for dV/dx = -E(x)

   Plotter plotter;
   plotter.plot(cd);       // plot the results in turn

#endif

   return 0;
}