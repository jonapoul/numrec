#include <vector>
#include <sstream>
#include <string> 

#include "../global.h"
#include "Plotter.h"
#include "RootFinder.h"
#include "PNJunction.h"

#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

Plotter::Plotter(int flag, char f_char) {
   output_flag = flag;
   format = "  ";
   format[1] = f_char;
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

   if (output_flag == SAVE_FIG)
      printf("Plots saved in cp2/fig/\n");
}

void Plotter::plot_rho(const PNJunction& pn) {
   plt::clf();
   format[0] = 'r';
   Points rho = pn.rho_points;
   plt::named_plot(rho.name, rho.x, rho.y, format);
   
   plt::xlim(pn.limit0, pn.limit4);
   plt::ylabel("rho(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Charge Density across a PN Junction; N = " << pn.N_points;
   plt::title(title.str());
   //plt::legend();
   (output_flag == SAVE_FIG) ? plt::save(filepath("1_rho.png")) : plt::show();
}

void Plotter::plot_E(const PNJunction& pn) {
   plt::clf();
   for (size_t i = 0; i < pn.E_points.size(); i++) {
      format[0] = colours[i % colours.length()];
      Points E = pn.E_points[i];
      plt::named_plot(E.name, E.x, E.y, format);
   }
   plt::xlim(pn.limit0, pn.limit4);
   plt::ylabel("E(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Electric Field across a PN Junction; N = " << pn.N_points;
   plt::title(title.str());
   plt::legend();
   (output_flag == SAVE_FIG) ? plt::save(filepath("2_E.png")) : plt::show();
}

void Plotter::plot_V(const PNJunction& pn) {
   plt::clf();
   for (size_t i = 0; i < pn.V_points.size(); i++) {
      format[0] = colours[i % colours.length()];
      Points V = pn.V_points[i];
      plt::named_plot(V.name, V.x, V.y, format);
   }

   plt::xlim(pn.limit0, pn.limit4);
   plt::ylabel("V(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Voltage across a PN Junction; N = " << pn.N_points;
   plt::title(title.str());      
   plt::legend();
   (output_flag == SAVE_FIG) ? plt::save(filepath("3_V.png")) : plt::show();
}

void Plotter::plot_E_differences(const PNJunction& pn) {
   plt::clf();
   format[0] = 'r';
   Points diff = pn.E_points[EULER] - pn.E_points[RK4];
   plt::named_plot("Euler-RK4", diff.x, diff.y, format);
   format[0] = 'b';
   diff = pn.E_points[RK2] - pn.E_points[RK4];
   plt::named_plot("RK2-RK4", diff.x, diff.y, format);

   plt::xlim(pn.limit0, pn.limit4);
   plt::ylabel("dE(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Difference in integrated Electric Field; N = " << pn.N_points;
   plt::title(title.str());      
   plt::legend();
   (output_flag == SAVE_FIG) ? plt::save(filepath("4_diffE.png")) : plt::show();
}

void Plotter::plot_V_differences(const PNJunction& pn) {
   plt::clf();
   format[0] = 'r';
   Points diff = pn.V_points[EULER] - pn.V_points[RK4];
   plt::named_plot("Euler-RK4", diff.x, diff.y, format);
   format[0] = 'b';
   diff = pn.V_points[RK2] - pn.V_points[RK4];
   plt::named_plot("RK2-RK4", diff.x, diff.y, format);

   double xmin = pn.limit0, xmax = pn.limit4;
   plt::xlim(xmin, xmax);
   plt::ylabel("dV(x)");
   plt::xlabel("x");
   std::stringstream title;
   title << "Difference in integrated Voltage; N = " << pn.N_points;
   plt::title(title.str());      
   plt::legend();
   (output_flag == SAVE_FIG) ? plt::save(filepath("5_diffV.png")) : plt::show();
}

void Plotter::plot_euler(const PNJunction& pn) {
   plt::clf();
   std::vector< Points > points = {pn.rho_points, 
                                   pn.E_points[EULER], 
                                   pn.V_points[EULER]};
   for (size_t i = 0; i < points.size(); i++) {
      format[0] = colours[i % colours.length()];
      plt::named_plot(points[i].name, points[i].x, points[i].y, format);
   }
   plt::xlim(pn.limit0, pn.limit4);
   std::stringstream title;
   title << "Euler results; N = " << pn.N_points;
   plt::title(title.str());      
   plt::legend();
   (output_flag == SAVE_FIG) ? plt::save(filepath("6_euler.png")) : plt::show();
}

void Plotter::plot_rk2(const PNJunction& pn) {
   plt::clf();
   std::vector< Points > points = {pn.rho_points, 
                                   pn.E_points[RK2], 
                                   pn.V_points[RK2]};
   for (size_t i = 0; i < points.size(); i++) {
      format[0] = colours[i % colours.length()];
      plt::named_plot(points[i].name, points[i].x, points[i].y, format);
   }
   plt::xlim(pn.limit0, pn.limit4);
   std::stringstream title;
   title << "RK2 results; N = " << pn.N_points;
   plt::title(title.str());      
   plt::legend();
   (output_flag == SAVE_FIG) ? plt::save(filepath("7_rk2.png")) : plt::show();
}

void Plotter::plot_rk4(const PNJunction& pn) {
   plt::clf();
   std::vector< Points > points = {pn.rho_points, 
                                   pn.E_points[RK4], 
                                   pn.V_points[RK4]};
   for (size_t i = 0; i < points.size(); i++) {
      format[0] = colours[i % colours.length()];
      plt::named_plot(points[i].name, points[i].x, points[i].y, format);
   }
   plt::xlim(pn.limit0, pn.limit4);
   std::stringstream title;
   title << "RK4 results; N = " << pn.N_points;
   plt::title(title.str());      
   plt::legend();
   (output_flag == SAVE_FIG) ? plt::save(filepath("8_rk4.png")) : plt::show();
}

inline std::string Plotter::filepath(const char* filename) const {
   char buf[100];
   snprintf(buf, 100, "%s/%s", fig_dir.c_str(), filename);
   return std::string(buf);
}