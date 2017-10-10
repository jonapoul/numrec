#ifndef ODESOLVER_H
#define ODESOLVER_H

#include <string>

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

private:
   ODEFunction function_option;
   std::string function_name;
   double xmin;
   double xmax;

   double dfdx(double x) const;
   double f   (double x) const;
};

#endif