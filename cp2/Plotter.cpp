#include <vector>
#include <sstream>
#include <fstream>
#include <string> 

#include "../global.h"
#include "Plotter.h"
#include "RootFinder.h"
#include "PNJunction.h"

#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

Plotter::Plotter(int flag, char f_char) {
   m_output_flag = flag;
   m_format = "  ";
   m_format[1] = f_char;
}

void Plotter::plot(const PNJunction& pn) {
   plot_rho(pn);
   plot_E(pn);
   plot_V(pn);

   plot_E_differences(pn);
   plot_V_differences(pn);

   plot_euler(pn);
   plot_rk2(pn);
   plot_rk4(pn);

   if (m_output_flag == SAVE_FIG)
      printf("Plots saved in cp2/fig/\n");
}

void Plotter::plot_rho(const PNJunction& pn) {
   plt::clf();
   m_format[0] = 'r';
   Points rho = pn.rho_points();
   plt::named_plot(rho.name, rho.x, rho.y, m_format);
   
   plt::xlim(pn.xmin(), pn.xmax());
   plt::ylabel("rho(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Charge Density across a PN Junction; N = " << pn.N_points();
   plt::title(title.str());
   //plt::legend();
   (m_output_flag == SAVE_FIG) ? plt::save(filepath("1_rho.png")) : plt::show();
}

void Plotter::plot_E(const PNJunction& pn) {
   plt::clf();
   for (size_t i = 0; i < METHOD_COUNT; i++) {
      m_format[0] = m_colours[i % m_colours.length()];
      Points E = pn.E_points(i);
      plt::named_plot(E.name, E.x, E.y, m_format);
   }
   plt::xlim(pn.xmin(), pn.xmax());
   plt::ylabel("E(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Electric Field across a PN Junction; N = " << pn.N_points();
   plt::title(title.str());
   plt::legend();
   (m_output_flag == SAVE_FIG) ? plt::save(filepath("2_E.png")) : plt::show();
}

void Plotter::plot_V(const PNJunction& pn) {
   plt::clf();
   for (size_t i = 0; i < METHOD_COUNT; i++) {
      m_format[0] = m_colours[i % m_colours.length()];
      Points V = pn.V_points(i);
      plt::named_plot(V.name, V.x, V.y, m_format);
   }

   plt::xlim(pn.xmin(), pn.xmax());
   plt::ylabel("V(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Voltage across a PN Junction; N = " << pn.N_points();
   plt::title(title.str());      
   plt::legend();
   (m_output_flag == SAVE_FIG) ? plt::save(filepath("3_V.png")) : plt::show();
}

void Plotter::plot_E_differences(const PNJunction& pn) {
   plt::clf();
   m_format[0] = 'r';
   Points diff = pn.E_points(EULER) - pn.E_points(RK4);
   plt::named_plot("Euler-RK4", diff.x, diff.y, m_format);
   m_format[0] = 'b';
   diff = pn.E_points(RK2) - pn.E_points(RK4);
   plt::named_plot("RK2-RK4", diff.x, diff.y, m_format);

   plt::xlim(pn.xmin(), pn.xmax());
   plt::ylabel("dE(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Difference in integrated Electric Field; N = " << pn.N_points();
   plt::title(title.str());      
   plt::legend();
   (m_output_flag == SAVE_FIG) ? plt::save(filepath("4_diffE.png")) : plt::show();
}

void Plotter::plot_V_differences(const PNJunction& pn) {
   plt::clf();
   m_format[0] = 'r';
   Points diff = pn.V_points(EULER) - pn.V_points(RK4);
   plt::named_plot("Euler-RK4", diff.x, diff.y, m_format);
   m_format[0] = 'b';
   diff = pn.V_points(RK2) - pn.V_points(RK4);
   plt::named_plot("RK2-RK4", diff.x, diff.y, m_format);

   double xmin = pn.xmin(), xmax = pn.xmax();
   plt::xlim(xmin, xmax);
   plt::ylabel("dV(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Difference in integrated Voltage; N = " << pn.N_points();
   plt::title(title.str());      
   plt::legend();
   (m_output_flag == SAVE_FIG) ? plt::save(filepath("5_diffV.png")) : plt::show();
}

void Plotter::plot_euler(const PNJunction& pn) {
   plt::clf();
   std::vector< Points > points = {pn.rho_points(), 
    pn.E_points(EULER), 
    pn.V_points(EULER)};
    for (size_t i = 0; i < points.size(); i++) {
      m_format[0] = m_colours[i % m_colours.length()];
      plt::named_plot(points[i].name, points[i].x, points[i].y, m_format);
   }
   plt::xlim(pn.xmin(), pn.xmax());
   std::stringstream title;
   title << "Euler results; N = " << pn.N_points();
   plt::title(title.str());      
   plt::legend();
   (m_output_flag == SAVE_FIG) ? plt::save(filepath("6_euler.png")) : plt::show();
}

void Plotter::plot_rk2(const PNJunction& pn) {
   plt::clf();
   std::vector< Points > points = {pn.rho_points(), 
    pn.E_points(RK2), 
    pn.V_points(RK2)};
    for (size_t i = 0; i < points.size(); i++) {
      m_format[0] = m_colours[i % m_colours.length()];
      plt::named_plot(points[i].name, points[i].x, points[i].y, m_format);
   }
   plt::xlim(pn.xmin(), pn.xmax());
   std::stringstream title;
   title << "RK2 results; N = " << pn.N_points();
   plt::title(title.str());      
   plt::legend();
   (m_output_flag == SAVE_FIG) ? plt::save(filepath("7_rk2.png")) : plt::show();
}

void Plotter::plot_rk4(const PNJunction& pn) {
   plt::clf();
   std::vector< Points > points = {pn.rho_points(), 
    pn.E_points(RK4), 
    pn.V_points(RK4)};
    for (size_t i = 0; i < points.size(); i++) {
      m_format[0] = m_colours[i % m_colours.length()];
      plt::named_plot(points[i].name, points[i].x, points[i].y, m_format);
   }
   plt::xlim(pn.xmin(), pn.xmax());
   std::stringstream title;
   title << "RK4 results; N = " << pn.N_points();
   plt::title(title.str());      
   plt::legend();
   (m_output_flag == SAVE_FIG) ? plt::save(filepath("8_rk4.png")) : plt::show();
}

inline std::string Plotter::filepath(const char* filename) const {
   char buf[100];
   snprintf(buf, 100, "%s/%s", m_fig_dir.c_str(), filename);
   return std::string(buf);
}

void Plotter::plot_tau() const {
   plt::clf();
   std::ifstream file;
   file.open("/home/jon/Downloads/tau.txt");
   std::vector<double> x, H2, H2new, H2SS, dNH1dt, tau;
   double i = 0;
   while (file) {
      double temp;
      // file >> temp;
      // x.push_back(temp);
      // file >> temp;
      // H2.push_back(temp);
      // file >> temp;
      // H2new.push_back(temp);
      // file >> temp;
      // H2SS.push_back(temp);
      // file >> temp;
      // dNH1dt.push_back(log10(abs(temp)));
      x.push_back(i++);
      file >> temp;
      tau.push_back(log10(temp));
   }
   x.pop_back();
   H2.pop_back();
   H2new.pop_back();
   H2SS.pop_back();
   dNH1dt.pop_back();
   tau.pop_back();
   file.close();

   //plt::ylim(785.0, 786.0);
   // plt::ylim(773.0, 774.0);
   // plt::named_plot("H2",     x, H2,     "r-");
   // plt::named_plot("H2new",  x, H2new,  "g-");
   // plt::named_plot("H2SS",   x, H2SS,   "b-");
   //plt::named_plot("log10( |dNH1dt| )", x, dNH1dt, "m");
   plt::named_plot("log10(tau)",    x, tau,    "k");
   plt::legend();
   plt::show();
}