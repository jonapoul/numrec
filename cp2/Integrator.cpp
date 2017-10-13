#include "../global.h"
#include "Integrator.h"
#include "PNJunction.h"

Integrator::Integrator(PNJunction* chargedist) {
   cd    = chargedist;
   xmax  = cd->limit4;
   xmin  = cd->limit0;
   delta = cd->delta;
}

void Integrator::integrate_for_E() {
   cd->E_points[EULER] = euler(dEdx_eq_RHO);
   cd->E_points[RK2]   = rk2(dEdx_eq_RHO);
   cd->E_points[RK4]   = rk4(dEdx_eq_RHO);
}

void Integrator::integrate_for_V() {
   // cd->V_points[EULER] = euler(dVdx_eq_NegE);
   // cd->V_points[RK2]   = rk2(dVdx_eq_NegE);
   // cd->V_points[RK4]   = rk4(dVdx_eq_NegE);
}

Points Integrator::euler(int ode) {
   array x = cd->rho_points.x;
   array y(cd->N_points);
   double x0index = cd->x0index;
   y[x0index] = (ode == dEdx_eq_RHO) ? cd->E0 : cd->V0;

   // from x0 UP to xmax
   for (int i = x0index+1; i < cd->N_points; i++)
      y[i] = y[i-1] + delta*dfdx(x[i-1], ode);

   // from x0 DOWN to xmin
   for (int i = x0index-1; i >= 0; i--)
      y[i] = y[i+1] - delta*dfdx(x[i+1], ode);

   std::string prefix = (ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_euler";
   return Points(x, y, name);
}

Points Integrator::rk2(int ode) {
   array x = cd->rho_points.x;
   array y(cd->N_points);
   double x0index = cd->x0index;
   y[x0index] = (ode == dEdx_eq_RHO) ? cd->E0 : cd->V0;

   // calculate y values from x0 up to xmax
   for (int i = x0index+1; i < cd->N_points; i++) {
      double xmid = x[i-1] + delta/2.0;
      double ymid = y[i-1] + dfdx(x[i-1], ode)*(delta/2.0);
      y[i] = y[i-1] + dfdx(xmid, ymid)*delta;
   }
   // same from x0 down to xmin
   for (int i = x0index-1; i >= 0; i--) {
      double xmid = x[i+1] - delta/2.0;
      double ymid = y[i+1] - dfdx(x[i+1], y[i+1])*(delta/2.0);
      y[i] = y[i+1] - dfdx(xmid, ymid)*(delta);
   }
   std::string prefix = (ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_rk2";
   return Points(x, y, name);
}

Points Integrator::rk4(int ode) {
   array x = cd->rho_points.x;
   array y(cd->N_points);
   double x0index = cd->x0index;
   y[x0index] = (ode == dEdx_eq_RHO) ? cd->E0 : cd->V0;

   // calculate y values from x0 up to xmax
   for (int i = x0index+1; i < cd->N_points; i++) {
      double k1 = dfdx(x[i-1],             ode);
      double k2 = dfdx(x[i-1] + delta/2.0, ode);
      double k3 = dfdx(x[i-1] + delta/2.0, ode);
      double k4 = dfdx(x[i-1] + delta,     ode);
      y[i] = y[i-1] + delta*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
   }
   // same from x0 down to xmin
   for (int i = x0index-1; i >= 0; i--) {
      double k1 = dfdx(x[i+1],             ode);
      double k2 = dfdx(x[i+1] - delta/2.0, ode);
      double k3 = dfdx(x[i+1] - delta/2.0, ode);
      double k4 = dfdx(x[i+1] - delta,     ode);
      y[i] = y[i+1] - delta*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
   }
      
   std::string prefix = (ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_rk4";
   return Points(x, y, name);
}

double Integrator::dfdx(double x, int ode) {
   switch (ode) {
      case dEdx_eq_RHO:  return cd->rho(x);
      case dVdx_eq_NegE: return 0;
      default:           return 0;
   }
}