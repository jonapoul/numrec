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
   // ./bin/checkpoint2 <function_option> <epsilon> <xmin> <xmax>
   RootFinder rf(argc, argv);
   rf.test_methods();
   plotter.plot(rf);
#endif
   
   // ./bin/checkpoint2 <function_option> <N_points> <xmin> <xmax>
   ODESolver ode(argc, argv);
   ode.integrate();
   plotter.plot(ode);
   plotter.plot_differences(ode);

#if 0
   ChargeDistribution cd;
   plotter.plot(cd);
#endif
   
   return 0;
}