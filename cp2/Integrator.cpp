#include "../global.h"
#include "Integrator.h"
#include "PNJunction.h"

Integrator::Integrator(PNJunction* junction) {
   // pointer to PNJunction object, in case we need things like boundary conditions
   pn    = junction;
   delta = junction->delta;
}

void Integrator::integrate_for_E() {
   // so that the dfdx(x) function knows what to do
   this->current_ode = ODE::dEdx_eq_RHO;

   pn->E_points[EULER] = euler();
   pn->E_points[RK2]   = rk2();
   pn->E_points[RK4]   = rk4();
}

void Integrator::integrate_for_V() {
   // so that the dfdx(x) function knows what to do
   this->current_ode = ODE::dVdx_eq_NegE;

   pn->V_points[EULER] = euler();
   pn->V_points[RK2]   = rk2();
   pn->V_points[RK4]   = rk4();
}

Points Integrator::euler() {
   // used by dfdx(x)
   this->current_algorithm = EULER;

   std::vector<double> x = pn->rho_points.x;
   std::vector<double> y(pn->N_points);
   double x0index = pn->x0index;
   y[x0index] = (current_ode == dEdx_eq_RHO) ? pn->E0 : pn->V0;

   // from x0 UP to xmax
   for (int i = x0index+1; i < pn->N_points; i++) {
      y[i] = y[i-1] + delta*dfdx(x[i-1]);
   }

   // from x0 DOWN to xmin
   for (int i = x0index-1; i >= 0; i--)
      y[i] = y[i+1] - delta*dfdx(x[i+1]);

   std::string prefix = (current_ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_euler";
   return Points(x, y, name);
}

Points Integrator::rk2() {
   // used by dfdx(x)
   this->current_algorithm = RK2;

   std::vector<double> x = pn->rho_points.x;
   std::vector<double> y(pn->N_points);
   double x0index = pn->x0index;
   y[x0index] = (current_ode == dEdx_eq_RHO) ? pn->E0 : pn->V0;

   // calculate y values from x0 up to xmax
   for (int i = x0index+1; i < pn->N_points; i++) {
      double xmid = x[i-1] + delta/2.0;
      //double ymid = y[i-1] + dfdx(x[i-1])*(delta/2.0);
      y[i] = y[i-1] + dfdx(xmid)*delta;
   }
   // same from x0 down to xmin
   for (int i = x0index-1; i >= 0; i--) {
      double xmid = x[i+1] - delta/2.0;
      //double ymid = y[i+1] - dfdx(x[i+1])*(delta/2.0);
      y[i] = y[i+1] - dfdx(xmid)*(delta);
   }
   std::string prefix = (current_ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_rk2";
   return Points(x, y, name);
}
 
Points Integrator::rk4() {
   // used by dfdx(x)
   this->current_algorithm = RK4;

   std::vector<double> x = pn->rho_points.x;
   std::vector<double> y(pn->N_points);
   double x0index = pn->x0index;
   y[x0index] = (current_ode == dEdx_eq_RHO) ? pn->E0 : pn->V0;

   // calculate y values from x0 up to xmax
   for (int i = x0index+1; i < pn->N_points; i++) {
      double k1 = dfdx(x[i-1]);
      double k2 = dfdx(x[i-1] + delta/2.0);
      double k3 = dfdx(x[i-1] + delta/2.0);
      double k4 = dfdx(x[i-1] + delta);
      y[i] = y[i-1] + delta*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
   }
   // same from x0 down to xmin
   for (int i = x0index-1; i >= 0; i--) {
      double k1 = dfdx(x[i+1]);
      double k2 = dfdx(x[i+1] - delta/2.0);
      double k3 = dfdx(x[i+1] - delta/2.0);
      double k4 = dfdx(x[i+1] - delta);
      y[i] = y[i+1] - delta*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
   }
      
   std::string prefix = (current_ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_rk4";
   return Points(x, y, name);
}

double Integrator::dfdx(double x) {
   switch (this->current_ode) {
      case dEdx_eq_RHO:  return pn->rho(x);
      case dVdx_eq_NegE: return pn->E(x, this->current_algorithm) * -1.0;
      default:           return 0;
   }
}