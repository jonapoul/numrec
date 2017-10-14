#include <iostream>
#include <string>
#include <math.h>

#include "../global.h"

// exercises
#include "RootFinder.h"

// checkpoint
#include "PNJunction.h"
#include "Integrator.h"
#include "Plotter.h"

int main() {
   int N_points = 50; // number of points
   double rho_limits[5] = {-2.0, -1.0, 0.0, 1.0, 2.0}; // limits of rho(x)
   
   // integration boundary conditions
   double x0 = -2.0;
   double E0 =  0.0;
   double V0 =  0.0;

   PNJunction pn(N_points, rho_limits, x0, E0, V0);
   pn.initialise_points(); // calculating values of rho(x) at each point

   Integrator ig(&pn);     // performs the integration, stores the results in the PNJunction object
   ig.integrate_for_E();   // use each of the 3 methods to solve dE/dx = rho(x)
   ig.integrate_for_V();   // same for dV/dx = -E(x)

   Plotter plotter(SAVE_FIG, '-'); // either SAVE_FIG or SHOW_FIG
   plotter.plot(pn); // plot the results and compare

   return 0;
}