#include <string>

#include "../global.h"
#include "Plotter.h"
#include "ChargeDistribution.h"

ChargeDistribution::ChargeDistribution() {
   this->limit0   = -2;  // xmin
   this->limit1   = -1;
   this->limit2   =  0;
   this->limit3   =  1;
   this->limit4   =  2;  // xmax
   this->k        =  M_PI/(limit3-limit1); // ????????????????
   this->N_points =  100;
   this->delta    = (limit4-limit0)/static_cast<double>(N_points);
}

void ChargeDistribution::generate_x_values() {
   x_range = array(N_points);
   for (int i = 0; i < N_points; i++) {
      x_range[i] = x0 + (i*delta);
   }
}

void ChargeDistribution::calculate_rho_values() {
   rho_range = array(N_points);
   for (int i = 0; i < N_points; i++) {
      rho_range[i] = rho(x_range[i]);
   }
}

void ChargeDistribution::integrate_for_E() {
   this->current_ode = ELECTRICFIELD;
   E_euler    = euler();
   E_rk2      = rk2();
   E_rk4      = rk4();
   E_analytic = analytic();
   Plotter::test(x_range, {E_euler, E_analytic}, 
                 {"euler", "analytic"}, 
                 limit0, limit4);
}

double ChargeDistribution::rho_shape(double x_0, double x_1, double x) const {
   double z = (x-x_0)/(x_1-x_0);
   return z*z*(exp(1-z)-1) / 0.18;
}

double ChargeDistribution::rho(double x) const {
   if      (x < limit1) return  0;
   else if (x < limit2) return  rho_shape(limit1, limit2, x);
   else if (x < limit3) return -rho_shape(limit3, limit2, x);
   else /*if (x < x4)*/       return  0;
}

double ChargeDistribution::E(int index, int method) const {
   switch (method) {
      case EULER:    return E_euler[index];
      case RK2:      return E_rk2[index];
      case RK4:      return E_rk4[index];
      case ANALYTIC: return E_analytic[index];
   }
   return 0; // just in case
}

double ChargeDistribution::V(int index, int method) const {
   switch (method) {
      case EULER:    return V_euler[index];
      case RK2:      return V_rk2[index];
      case RK4:      return V_rk4[index];
      case ANALYTIC: return V_analytic[index];
   }
   return 0; // just in case
}

double ChargeDistribution::dfdx(int index, int method) const {
   if (current_ode == ELECTRICFIELD) {
      // dE/dx = rho(x)
      return rho_range[index];
   } else { // current_ode == VOLTAGE
      // dV/dx = -E(x)
      switch (method) {
         case EULER:    return -E_euler[index];
         case RK2:      return -E_rk2[index];
         case RK4:      return -E_rk4[index];
         case ANALYTIC: return -E_analytic[index];
      }
      return 0;
   }
}

array ChargeDistribution::euler() const {
   array y(N_points);
   y[0] = (current_ode == ELECTRICFIELD) ? E0 : V0; // boundary condition
   for (int i = 1; i < N_points; i++) {
      y[i] = y[i-1] + delta * dfdx(i, EULER);
   }
   return y;
}

array ChargeDistribution::rk2() const {

   // for (int i = this->x0_index+1; i < this->x_size; i++) {
   //    double xmid = output.x[i-1] + this->delta/2.0;
   //    double ymid = output.y[i-1] + dfdx(output.x[i-1], output.y[i-1])*(this->delta/2.0);
   //    output.y[i] = output.y[i-1] + dfdx(xmid, ymid)*delta;
   // }

   array y(N_points);
   y[0] = (current_ode == ELECTRICFIELD) ? E0 : V0; // boundary condition
   for (int i = 1; i < N_points; i++) {
      //double xmid = x_range[i-1] + delta/2.0;
      //double ymid = y[i-1] + dfdx(i, EULER)*(delta/2.0);
      y[i] = 0;
   }
   return {};
}

array ChargeDistribution::rk4() const {
   return {};
}

array ChargeDistribution::analytic() const {
   // all this from dumping into wolfram
   array y(N_points);

   // integration constants, calculated using boundary conditions x0, E0, V0
   static const double c_E = E0 - (1./0.18)*( exp(x0+2)*(x0*x0+1) - pow(x0+1, 3)/3.0 );
   static const double c_V = V0-(pow(x0+1,4)-12*exp(x0+2)*(x0*x0-2*x0+3))/(12*0.18) + c_E*x0;

   if (current_ode == ELECTRICFIELD) {
      for (int i = 0; i < N_points; i++) {
         double x = x_range[i];
         y[i] = (1./0.18)*( exp(x+2)*(x*x+1) - pow(x+1, 3)/3.0 ) + c_E;
         if (i > N_points/2)
            y[i] *= -1;
      }
   } else { // current_ode == VOLTAGE
      for (int i = 0; i < N_points; i++) {
         double x = x_range[i];
         y[i] = (pow(x+1,4) - 12*exp(x+2)*(x*x-2*x+3))/(12*0.18) + c_E*x + c_V;
         if (i > N_points/2)
            y[i] *= -1;
      }
   }
   return y;
}