#ifndef CHARGEDISTRIBUTION_H
#define CHARGEDISTRIBUTION_H

#include <vector>
#include <math.h>
#include <string>
#include "../global.h"
#include "Points.h"

class PNJunction {
public:
   
   PNJunction(int N, double* boundaries, double x_0, double E_0, double V_0);
   void initialise_points();   // fill out x/y points for y=rho(x)
   double rho(double x) const; // calculate charge density at position x

   Points& rho_points()            { return m_rho_points; }
   Points  rho_points() const        { return m_rho_points; }
   Points& E_points(int alg)       { return m_E_points[alg]; }
   Points  E_points(int alg) const { return m_E_points[alg]; }
   Points& V_points(int alg)       { return m_V_points[alg]; }
   Points  V_points(int alg) const { return m_V_points[alg]; }

   double xmin() const     { return m_limit0; }
   double xmax() const     { return m_limit4; }
   double N_points() const { return m_N_points; }
   double delta() const    { return m_delta; }
   double V0() const       { return m_V0; }
   double E0() const       { return m_E0; }
   int x0index() const     { return m_x0index; }

private:
   // specifying ranges of the charge density
   double m_limit0; // rho = 0         for m_limit0 < x < m_limit1
   double m_limit1; // rho = +shape(x) for m_limit1 < x < m_limit2
   double m_limit2; // rho = -shape(x) for m_limit2 < x < m_limit3
   double m_limit3; // rho = 0         for m_limit3 < x < m_limit4
   double m_limit4;
   int m_N_points;  // number of x/y coordinates
   double m_delta;  // x distance between points

   double m_x0;   // boundary condition x
   int m_x0index; // index of x0 in the points.x arrays
   double m_E0;   // boundary condition E(x0) = 0
   double m_V0;   // boundary condition V(x0) = 0

   Points m_rho_points;            // analytic values of rho(x)
   std::vector<Points> m_E_points; // coords of each E(x) value, for each integration method
   std::vector<Points> m_V_points; // coords of each V(x) value, for each integration method

   double rho_shape(double x0, double x1, double x) const; // return shape of rho(x) in each region
};

#endif