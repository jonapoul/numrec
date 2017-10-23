#include "../global.h"
#include "Integrator.h"
#include "PNJunction.h"

Integrator::Integrator(PNJunction* junction)
      : m_pn(junction){ }

void Integrator::integrate_for_E() {
   // so that the dfdx(x) function knows what to do
   this->m_current_ode = dEdx_eq_RHO;

   m_pn->E_points(EULER) = euler();
   m_pn->E_points(RK2)   = rk2();
   m_pn->E_points(RK4)   = rk4();
}

void Integrator::integrate_for_V() {
   // so that the dfdx(x) function knows what to do
   this->m_current_ode = dVdx_eq_NegE;

   m_pn->V_points(EULER) = euler();
   m_pn->V_points(RK2)   = rk2();
   m_pn->V_points(RK4)   = rk4();
}

Points Integrator::euler() {
   // used by dfdx(x)
   this->m_current_algorithm = EULER;

   std::vector<double> x = m_pn->rho_points().x;
   std::vector<double> y(m_pn->N_points());
   double x0index = m_pn->x0index();
   double dx = m_pn->delta();
   y[x0index] = (m_current_ode == dEdx_eq_RHO) ? m_pn->E0() : m_pn->V0();

   // from x0 UP to xmax
   for (int i = x0index+1; i < m_pn->N_points(); i++) {
      y[i] = y[i-1] + dx*dfdx(x[i-1]);
   }
   // from x0 DOWN to xmin
   for (int i = x0index-1; i >= 0; i--) {
      y[i] = y[i+1] - dx*dfdx(x[i+1]);
   }

   std::string prefix = (m_current_ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_euler";
   return Points(x, y, name);
}

Points Integrator::rk2() {
   // used by dfdx(x)
   this->m_current_algorithm = RK2;

   std::vector<double> x = m_pn->rho_points().x;
   std::vector<double> y(m_pn->N_points());
   double x0index = m_pn->x0index();
   double dx = m_pn->delta();
   y[x0index] = (m_current_ode == dEdx_eq_RHO) ? m_pn->E0() : m_pn->V0();

   // calculate y values from x0 up to xmax
   for (int i = x0index+1; i < m_pn->N_points(); i++) {
      double xmid = x[i-1] + dx/2.0;
      y[i] = y[i-1] + dfdx(xmid)*dx;
   }
   // same from x0 down to xmin
   for (int i = x0index-1; i >= 0; i--) {
      double xmid = x[i+1] - dx/2.0;
      //double ymid = y[i+1] - dfdx(x[i+1])*(delta/2.0);
      y[i] = y[i+1] - dfdx(xmid)*dx;
   }
   std::string prefix = (m_current_ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_rk2";
   return Points(x, y, name);
}
 
Points Integrator::rk4() {
   // used by dfdx(x)
   this->m_current_algorithm = RK4;

   std::vector<double> x = m_pn->rho_points().x;
   std::vector<double> y(m_pn->N_points());
   double x0index = m_pn->x0index();
   double dx = m_pn->delta();
   y[x0index] = (m_current_ode == dEdx_eq_RHO) ? m_pn->E0() : m_pn->V0();

   // calculate y values from x0 up to xmax
   for (int i = x0index+1; i < m_pn->N_points(); i++) {
      double k1 = dfdx(x[i-1]);
      double k2 = dfdx(x[i-1] + dx/2.0);
      double k3 = dfdx(x[i-1] + dx/2.0);
      double k4 = dfdx(x[i-1] + dx);
      y[i] = y[i-1] + dx*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
   }
   // same from x0 down to xmin
   for (int i = x0index-1; i >= 0; i--) {
      double k1 = dfdx(x[i+1]);
      double k2 = dfdx(x[i+1] - dx/2.0);
      double k3 = dfdx(x[i+1] - dx/2.0);
      double k4 = dfdx(x[i+1] - dx);
      y[i] = y[i+1] - dx*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
   }
      
   std::string prefix = (m_current_ode == dEdx_eq_RHO) ? "E" : "V";
   std::string name = prefix + "_rk4";
   return Points(x, y, name);
}

double Integrator::dfdx(double x) {
   switch (this->m_current_ode) {
      case dEdx_eq_RHO:  return m_pn->rho(x);
      case dVdx_eq_NegE: return -single_integrate_for_E(x);
      default:           return 0;
   }
}

/*
   XX is an arbitrary distance dx between two discrete x values in the array.
   dx is the distance between x0 and XX.
   E0 is the value of E at x0, we're integrating to find E(XX), using whichever method below.
*/
double Integrator::single_integrate_for_E(double XX) {
   Points points = m_pn->E_points(m_current_algorithm);
   // find index of the x coord immediately before XX
   int closest_index = 0;
   for (size_t i = 1; i < points.size(); i++) {
      // if the current x point has switched to larger than XX, save the previous index and break
      if (XX - points.x[i] < 0) {
         closest_index = i-1;
         break;
      }
   }
   double x0 = points.x[closest_index];
   double E0 = points.y[closest_index];
   
   // if we've already calculated E(XX), return that and dont bother integrating again
   if (abs(x0-XX) < 1e-8) {
      return E0;
   }

   // if we're carrying on here, we need a value of E halfway between our existing discrete points
   double dx = m_pn->delta()/2.0;

   // perform a single iteration of whatever method we're using at the moment
   switch (this->m_current_algorithm) {
      case EULER: {
         return E0 + m_pn->rho(x0)*dx;
      } 
      case RK2: {
         double xmid = x0 + dx/2.0;
         return E0 + m_pn->rho(xmid)*dx;
      } 
      case RK4: {
         double k1 = m_pn->rho(x0);
         double k2 = m_pn->rho(x0 + dx/2.0);
         double k3 = m_pn->rho(x0 + dx/2.0);
         double k4 = m_pn->rho(x0 + dx);
         return E0 + dx*(k1/6.0 + k2/3.0 + k3/3.0 + k4/6.0);
      } 
      default: {
         return 0;
      }
   }
}