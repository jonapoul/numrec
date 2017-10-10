#include <vector>
#include <sstream>
#include <string> 

#include "Plotter.h"
#include "RootFinder.h"
#include "ODESolver.h"
#include "ChargeDistribution.h"

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
   std::vector<double> x_yaxis = {0.0, 0.0},             y_yaxis = {-1e200, 1e200}; // y axis line

   std::stringstream ss;
   ss << rf.function_name << " function, N_points = " << N_points;
   plt::title(ss.str());
   
   // axis limits
   double dw = (rf.xmax - rf.xmin)/10.0;
   double dh = (ymax - ymin)/10.0;
   plt::ylim(ymin-dh, ymax+dh);
   plt::xlim(rf.xmin-dw, rf.xmax+dw);
   
   plt::plot(x, y, "r-");                                         // curve
   plt::plot({rf.xmin-dw, rf.xmax+dw}, {0.0, 0.0},         "b-"); // x axis
   plt::plot({0.0, 0.0},               {ymin-dh, ymax+dh}, "b-"); // y axis
   plt::xlabel("x");
   plt::ylabel("f(x)");
   if (show_plot)
      plt::show();
}

void Plotter::plot(const ODESolver& ode, bool show_plot) {
   int N_points = 1e4;
   std::vector<double> x(N_points), y(N_points), dydx(N_points), xaxis(N_points), yaxis(N_points);
   double ymin = 1e200, ymax = -1e200;

   for (int i = 0; i < N_points; i++) {
      // calculate y and dy/dx values at each point in the range
      x[i]    = ode.xmin + i*(ode.xmax-ode.xmin)/(double)N_points;
      y[i]    = ode.f(x[i]);
      dydx[i] = ode.dfdx(x[i]);

      // update the min/max yvalues for axis limits
      if (y[i]    < ymin) ymin = y[i];
      if (y[i]    > ymax) ymax = y[i];
      if (dydx[i] < ymin) ymin = dydx[i];
      if (dydx[i] > ymax) ymax = dydx[i];
   }

   // axis limits
   double dw = (ode.xmax - ode.xmin)/10.0;
   double dh = (ymax - ymin)/10.0;
   plt::xlim(ode.xmin-dw, ode.xmax+dw);
   plt::ylim(ymin-dh,     ymax+dh);

   // two relevant curves
   plt::named_plot("dy/dx", x, dydx, "r-");
   plt::named_plot("y(x)",  x, y,    "g-");

   plt::plot({ode.xmin-dw, ode.xmax+dw}, {0.0, 0.0},         "b-"); // x axis
   plt::plot({0.0, 0.0},                 {ymin-dh, ymax+dh}, "b-"); // y axis
   plt::xlabel("x");

   std::stringstream ss;
   ss << ode.function_name << ", N_points = " << N_points;
   plt::title(ss.str());
   plt::legend();
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