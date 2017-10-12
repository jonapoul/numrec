#ifndef ODESOLVER_H
#define ODESOLVER_H

#include <string>
#include <vector>

#include "../global.h"

class ODESolver {
public:
   enum ODEFunction { QUADRATIC, Y_X, COS_X, RHO_X, E_X, FUNCTION_COUNT };
   enum ODEMethod   { EULER, RK2, RK4, ANALYTIC, METHOD_COUNT };
   friend class Plotter;

   ODESolver(ODEFunction f, int N, double min, double max, double x0, double y0);
   void integrate();

private:
   ODEFunction option;       // choice of initial ODE to solve
   std::string option_name;  //
   int N_points;             // determines spacing along x axis
   double delta;             //
   double xmin;              // limits of x axis
   double xmax;              //
   double x0;                // boundary conditions
   double y0;                // = y(x=x0)
   int x0_index;             // index of x0 in the arrays of x values
   int x_size;               // length of x array, not necessarily the same as N_points

   // for storing x/y coords of each point for each integration algorithm
   std::vector<CoordsArray> coords;
   // differences between eah result and the expected value
   std::vector<CoordsArray> differences;
   array x_range;

   double dfdx(const double x, const double y) const;
   double f   (const double x) const;

   CoordsArray euler(const array& x_values = {}) const;
   CoordsArray rk2(const array& x_values = {}) const;
   CoordsArray rk4(const array& x_values = {}) const;
   CoordsArray analytic() const;
   std::vector<CoordsArray> calculate_differences() const;
   array get_x_values();
};

#endif