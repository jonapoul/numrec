#include <string>

#include "../global.h"
#include "Points.h"
#include "PNJunction.h"

PNJunction::PNJunction(int N, double* boundaries, double x0, double E0, double V0)
      : m_N_points(N), m_x0(x0), m_E0(E0), m_V0(V0) {
   this->m_limit0   = boundaries[0];  // xmin
   this->m_limit1   = boundaries[1];
   this->m_limit2   = boundaries[2];
   this->m_limit3   = boundaries[3];
   this->m_limit4   = boundaries[4];  // xmax
   this->m_delta    = (m_limit4-m_limit0)/(double)N;

   // allocating space for the coordinate arrays
   this->m_rho_points.x = std::vector<double>(N, 0.0);
   this->m_rho_points.y = std::vector<double>(N, 0.0);
   this->m_E_points     = std::vector<Points>(METHOD_COUNT);
   this->m_V_points     = std::vector<Points>(METHOD_COUNT);

   // allocate each of the E and V arrays for each integration method
   for (int i = 0; i < METHOD_COUNT; i++) {
      this->m_E_points[i].x = std::vector<double>(N, 0.0);
      this->m_E_points[i].y = std::vector<double>(N, 0.0);

      this->m_V_points[i].x = std::vector<double>(N, 0.0);
      this->m_V_points[i].y = std::vector<double>(N, 0.0);
   }
}

void PNJunction::initialise_points() {
   // finding where x0 fits into the range and saving that array index
   this->m_x0index = (m_x0-m_limit0)/(m_limit4-m_limit0) * m_N_points;

   // fill out the points
   for (int i = 0; i < m_N_points; i++) {
      m_rho_points.x[i] = m_x0 + (i - m_x0index)*m_delta;
      m_rho_points.y[i] = rho(m_rho_points.x[i]);
   }
   m_rho_points.name = "rho";
}

double PNJunction::rho_shape(double x0, double x1, double x) const {
   double z = (x - x0) / (x1 - x0);
   return pow(z,2) * (exp(1-z)-1) / 0.18;
}

double PNJunction::rho(double x) const {
   // if      (x < m_limit1) return  0.0;
   // else if (x < m_limit2) return -1.0;
   // else if (x < m_limit3) return  1.0;
   // else                   return  0.0;

   if      (x < m_limit1) return  0.0;
   else if (x < m_limit2) return  rho_shape(m_limit1, m_limit2, x);
   else if (x < m_limit3) return -rho_shape(m_limit3, m_limit2, x);
   else                   return  0.0;
}