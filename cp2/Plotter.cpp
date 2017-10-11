#include <vector>
#include <sstream>
#include <string> 

#include "../global.h"
#include "Plotter.h"
#include "RootFinder.h"
#include "ODESolver.h"
#include "ChargeDistribution.h"

#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

void Plotter::plot(const RootFinder& rf) {
   int N_points = 1e3;
   array x(N_points), y(N_points);
   for (int i = 0; i < N_points; i++) {
      x[i] = rf.xmin + i*(rf.xmax-rf.xmin)/(double)N_points;
      y[i] = rf.f(x[i]);
   }

   std::stringstream ss;
   ss << rf.function_name << " function, epsilon = " << rf.epsilon << ", N_points = " << N_points;
   printf("%d %s\n", (int)rf.function_option, rf.function_name.c_str());
   plt::title(ss.str());
   
   // axis limits
   double ymin, ymax;
   Plotter::find_extrema({y}, &ymin, &ymax);
   double dw = (rf.xmax - rf.xmin)/10.0;
   double dh = (ymax - ymin)/10.0;
   plt::ylim(ymin-dh, ymax+dh);
   plt::xlim(rf.xmin-dw, rf.xmax+dw);
   
   plt::plot(x, y, "r-");                                         // curve
   plt::plot({rf.xmin-dw, rf.xmax+dw}, {0.0, 0.0},         "b-"); // x axis
   plt::plot({0.0, 0.0},               {ymin-dh, ymax+dh}, "b-"); // y axis
   plt::xlabel("x");
   plt::ylabel("f(x)");
   plt::show();
}

void Plotter::plot(const ODESolver& ode) {
   // axis limits
   double ymin, ymax;
   Plotter::find_extrema(ode.coords, &ymin, &ymax);
   double dw = (ode.xmax - ode.xmin)/10.0;
   double dh = (ymax - ymin)/10.0;
   plt::xlim(ode.xmin-dw, ode.xmax+dw);
   plt::ylim(ymin-dh,     ymax+dh);

   // formatting and plotting each line
   std::vector<char> colours = {'b', 'g', 'r', 'm', 'y', 'c'};
   std::string format = " .";
   for (int i = 0; i < ODE_COORDS_COUNT; i++) {
      format[0] = colours[i % colours.size()];
      if (i == static_cast<int>(ODECoords::ACTUAL)) format[1] = '-';
      plt::named_plot(ode.coords[i].name, ode.coords[i].x, ode.coords[i].y, format);
   }

   plt::plot({ode.xmin-dw, ode.xmax+dw}, {0.0, 0.0},         "k-"); // x axis
   plt::plot({0.0, 0.0},                 {ymin-dh, ymax+dh}, "k-"); // y axis

   std::stringstream ss;
   ss << "Integrated line of " << ode.option_name << '\n';
   ss << "N_points = " << ode.N_points << "; x0 = " << ode.x0 << "; y(x0) = " << ode.y0;
   ss << "; xmin = " << ode.xmin << "; xmax = " << ode.xmax;
   plt::xlabel("x");
   plt::ylabel("y");
   plt::title(ss.str());
   plt::legend();
   plt::show();
}

void Plotter::plot_differences(const ODESolver& ode) {
   // axis limits
   double ymin, ymax;
   Plotter::find_extrema({ode.differences[0]}, &ymin, &ymax);
   double dw = (ode.xmax - ode.xmin)/10.0;
   double dh = (ymax - ymin)/10.0;
   plt::xlim(ode.xmin-dw, ode.xmax+dw);
   plt::ylim(ymin-dh,     ymax+dh);

   // formatting and plotting each line
   std::vector<char> colours = {'b', 'g', 'r', 'm', 'y', 'c'};
   std::string format = " .";
   for (int i = 0; i < /*ACTUAL*/RK2; i++) {
      format[0] = colours[i % colours.size()];
      plt::named_plot(ode.differences[i].name, ode.differences[i].x, ode.differences[i].y, format);
   }

   plt::plot({ode.xmin-dw, ode.xmax+dw}, {0.0, 0.0},         "k-"); // x axis
   plt::plot({0.0, 0.0},                 {ymin-dh, ymax+dh}, "k-"); // y axis

   std::stringstream ss;
   ss << "Differences between integrated and actual for " << ode.option_name << '\n';
   ss << "N_points = " << ode.N_points << "; x0 = " << ode.x0 << "; y(x0) = " << ode.y0;
   ss << "; xmin = " << ode.xmin << "; xmax = " << ode.xmax;
   plt::xlabel("x");
   plt::ylabel("y");
   plt::title(ss.str());
   plt::legend();
   plt::show();
}

void Plotter::plot(const ChargeDistribution& cd) {
   array x, y;
   double min_value = -2, max_value = 2;
   int N_points = 1000;
   cd.get_XY_values(min_value, max_value, N_points, &x, &y);

   plt::named_plot("rho(x)", x, y, "r-");
   plt::xlabel("x");
   plt::ylabel("Charge Density");
   plt::xlim(min_value, max_value);
   plt::title("put title here");
   plt::legend();
   plt::show();
}


void Plotter::find_extrema(const std::vector<array>& y_arrays, double* min, double* max) {
   if (!min || !max) {
      printf("You tried passing a nullptr to Plotter::find_extrema(). Exiting...\n");
      exit(1);
   }
   *min = 1e200, *max = -1e200;
   for (auto array : y_arrays) {
      for (const auto& y : array) {
         if (y < *min) *min = y;
         if (y > *max) *max = y;
      }
   }
   return;
}

void Plotter::find_extrema(const std::vector<CoordsArray>& functions, 
                          double* min, double* max, bool use_x_instead) {
   if (!use_x_instead) {
     std::vector<array> y_arrays;
     for (const auto& f : functions) 
        y_arrays.push_back(f.y);
     
     find_extrema(y_arrays, min, max); 
   } else {
      std::vector<array> x_arrays;
      for (const auto& f : functions) 
         x_arrays.push_back(f.x);

      find_extrema(x_arrays, min, max);
   }
}
