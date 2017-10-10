#ifndef ODESOLVER_H
#define ODESOLVER_H

#include <string>
#include <vector>

#include "../global.h"

enum ODEFunction {
   QUADRATIC,
   Y_X,
   COS_X,
   ODE_FUNCTION_COUNT
};

class ODESolver {
public:
   friend class Plotter;

   ODESolver(int argc, char** argv);
   void test_methods() const;

private:
   ODEFunction function_option;
   std::string function_name;
   int N_points;
   double delta;
   double xmin;
   double xmax;
   double x0;
   double y0;

   double dfdx(double x) const;
   double f   (double x) const;

   void euler(double x0, double y0, array* x, array* y) const;
};

#endif