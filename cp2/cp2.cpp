#include <iostream>
#include <string>
#include <math.h>

// exercises
#include "RootFinder.h"
#include "ODESolver.h"
#include "Plotter.h"

// checkpoint
#include "ChargeDistribution.h"

int main(int argc, char** argv) {
   Plotter plotter;

#if 0
   // cp2 <function_option> <epsilon> <xmin> <xmax>
   RootFinder rf(argc, argv);
   rf.test_methods();
   plotter.plot(rf, true);
#endif
   
   ODESolver ode(argc, argv);
   ode.test_methods();
   plotter.plot(ode, true);

   /*
   PART 1 
      - dE/dx = rho(x)
      - determine E field from x=-2 to +2, where E(x=0)=0
      - use numerical method of integrating ODE from lectures
      - implement euler and Rk4 methods and compare
      - plot E(x) using both methods (together?)

   PART 2
      - dV/dx = -E(x)
      - determine voltage in range x=-2 to +2, where V(x=0)=0
      - input the same discrete x/y values for this integration
      - plot V(x) in range -2 to +2
   */
   ChargeDistribution cd;
   plotter.plot(cd, "Charge density against x", false);
}