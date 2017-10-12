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
   find_extrema({y}, x, rf.xmin, rf.xmax, &ymin, &ymax);
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
   find_extrema(ode.coords, ode.x_range, ode.xmin, ode.xmax, &ymin, &ymax);
   double dh = (ymax - ymin)/10.0;
   plt::xlim(ode.xmin, ode.xmax);
   plt::ylim(ymin-dh,  ymax+dh);

   // formatting and plotting each line
   std::vector<char> colours = {'b', 'g', 'r', 'y', 'm', 'c'};
   std::string format = " .";
   for (int i = 0; i < ODESolver::METHOD_COUNT; i++) {
      format[0] = colours[i % colours.size()];
      if (i == static_cast<int>(ODESolver::ANALYTIC)) format[1] = '-';
      plt::named_plot(ode.coords[i].name, ode.coords[i].x, ode.coords[i].y, format);
   }

   plt::plot({ode.xmin, ode.xmax}, {0.0, 0.0},         "k-"); // x axis
   plt::plot({0.0, 0.0},           {ymin-dh, ymax+dh}, "k-"); // y axis

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
   find_extrema(ode.differences, ode.x_range, ode.xmin, ode.xmax, &ymin, &ymax);
   double dh = (ymax - ymin)/10.0;
   plt::xlim(ode.xmin, ode.xmax);
   plt::ylim(ymin-dh,  ymax+dh);

   // formatting and plotting each line
   std::vector<char> colours = {'b', 'g', 'r', 'm', 'y', 'c'};
   std::string format = " .";
   for (int i = 0; i < ODESolver::ANALYTIC; i++) {
      format[0] = colours[i % colours.size()];
      std::string name = ode.differences[i].name;
      if (i == ODESolver::EULER) name += "/20";
      plt::named_plot(name, ode.differences[i].x, ode.differences[i].y, format);
   }

   plt::plot({ode.xmin, ode.xmax}, {0.0, 0.0},         "k-"); // x axis
   plt::plot({0.0, 0.0},           {ymin-dh, ymax+dh}, "k-"); // y axis

   std::stringstream ss;
   ss << "Differences between integrated and analytic for " << ode.option_name << '\n';
   ss << "N_points = " << ode.N_points << "; x0 = " << ode.x0 << "; y(x0) = " << ode.y0;
   ss << "; xmin = " << ode.xmin << "; xmax = " << ode.xmax;
   plt::xlabel("x");
   plt::ylabel("y");
   plt::title(ss.str());
   plt::legend();
   plt::show();
}

void Plotter::plot(const ChargeDistribution& cd) {
   // double xmin = cd.x0, xmax = cd.x4;
   // int N_points = 100;
   // array x, y;
   // cd.get_XY_values(xmin, xmax, N_points, &x, &y);

   // plt::named_plot("rho(x)", x, y, "r.");
   // plt::xlabel("x");
   // plt::ylabel("y");
   // plt::xlim(xmin, xmax);
   // plt::title("Charge Density and Electric Field");
   // plt::legend();
   // plt::show();
}


void Plotter::find_extrema(const std::vector<array>& y_arrays, const array& x, 
                           double xmin, double xmax, double* min, double* max) {
   if (!min || !max) {
      printf("You tried passing a nullptr to Plotter::find_extrema(). Exiting...\n");
      exit(1);
   }
   *min = 1e200, *max = -1e200;
   for (int i = 0; i < (int)y_arrays.size(); i++) {
      array y = y_arrays[i];
      for (int j = 0; j < (int)y_arrays[i].size(); j++) {
         if (x[j] < xmin) continue;
         if (x[j] > xmax) continue;
         if (y[j] < *min) *min = y[j];
         if (y[j] > *max) *max = y[j];
      }
   }
   return;
}

void Plotter::find_extrema(const std::vector<CoordsArray>& functions, const array& x,
                           double xmin, double xmax, double* min, double* max) {
   std::vector<array> y_arrays;
   for (const auto& f : functions) 
      y_arrays.push_back(f.y);
   
   find_extrema(y_arrays, x, xmin, xmax, min, max); 
}

void Plotter::test(array x, std::vector<array> y, 
                   std::vector<std::string> names,
                   double xmin, double xmax) {
   std::vector<char> colours = {'b', 'g', 'r', 'm', 'y', 'c'};
   std::string format = " .";
   for (int i = 0; i < (int)y.size(); i++) {
      format[0] = colours[i % colours.size()];
      plt::named_plot(names[i], x, y[i], format);
   }
   plt::xlim(xmin, xmax);
   double ymin, ymax;
   find_extrema(y, x, xmin, xmax, &ymin, &ymax);
   plt::ylim(ymin, ymax);
   plt::legend();
   plt::show();
}