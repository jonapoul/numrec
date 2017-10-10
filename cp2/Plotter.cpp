#include <vector>
#include <sstream>
#include <string> 

#include "Plotter.h"
#include "ChargeDistribution.h"
#include "RootFinder.h"

#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

void Plotter::plot(const RootFinder& rf, bool show_plot) {
   int N_points = 1e4;
   std::vector<double> x(N_points), y(N_points); // curve line
   double ymin = 1e200, ymax = -1e200;
   for (int i = 0; i < N_points; i++) {
      x[i] = rf.xmin + i*(rf.xmax-rf.xmin)/(double)N_points;
      y[i] = rf.f(x[i]);
      if (y[i] < ymin) ymin = y[i];
      if (y[i] > ymax) ymax = y[i];
   }
   std::vector<double> x_xaxis = {rf.xmin-1, rf.xmax+1}, y_xaxis = {0.0, 0.0};      // x axis line
   std::vector<double> x_yaxis = {0.0, 0.0},             y_yaxis = {-1e200, 1e200}; // y axis line

   std::stringstream ss;
   ss << rf.function_name << " function, N_points = " << N_points;
   plt::title(ss.str());
   plt::plot(x, y, "r-");             // curve
   plt::plot(x_xaxis, y_xaxis, "b-"); // x axis line
   plt::plot(x_yaxis, y_yaxis, "b-"); // y axis line

   double w = rf.xmax - rf.xmin; // width
   double h = ymax - ymin;       // height
   double dw = w/10.0, dh = h/10.0;
   plt::ylim(ymin-dh, ymax+dh);
   plt::xlim(rf.xmin-dw, rf.xmax+dw);
   plt::xlabel("x");
   plt::ylabel("f(x)");
   if (show_plot)
      plt::show();
}

void Plotter::plot(const ChargeDistribution& cd, const std::string& title, bool show_plot) {
   std::vector<double> x, y;
   double min_value = -2, max_value = 2;
   int N_points = 1000;
   cd.get_XY_values(min_value, max_value, N_points, &x, &y);

   plt::named_plot("rho(x)", x, y, "r-");
   plt::xlabel("x");
   plt::ylabel("Charge Density");
   plt::xlim(min_value, max_value);
   plt::title(title);
   plt::legend();
   if (show_plot)
      plt::show();
}