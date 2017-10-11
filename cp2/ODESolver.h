#ifndef ODESOLVER_H
#define ODESOLVER_H

#include <string>
#include <vector>

#include "../global.h"

class ODESolver {
public:
   friend class Plotter;

   ODESolver(int argc, char** argv);
   void integrate();

   // specifies which choice of dy/dx to solve. 
   // chosen via command line, defaults to QUADRATIC
   enum ODEFunction { QUADRATIC, Y_X, COS_X, FUNCTION_COUNT };

   // specifies which integration method to use.
   // each one is called in turn by test_methods(), then the coords stored in an array
   enum ODECoords { EULER, RK2, RK4, ANALYTIC, COORDS_COUNT };

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

   CoordsArray euler() const;
   CoordsArray rk2() const;
   CoordsArray rk4() const;
   CoordsArray analytic() const;
   std::vector<CoordsArray> calculate_differences() const;
   void get_arguments(int argc, char** argv);
   array get_x_values();
};

#endif