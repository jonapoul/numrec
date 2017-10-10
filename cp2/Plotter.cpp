#include <vector>
#include <sstream>
#include <string> 

#include "Plotter.h"
#include "ChargeDistribution.h"
#include "RootFinder.h"

#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

void Plotter::plot(const RootFinder& rf) {
   int N_points = 1e4;
   std::vector<double> x(N_points), y(N_points);
   for (int i = 0; i < N_points; i++) {
      x[i] = rf.xmin + i*(rf.xmax-rf.xmax)/(double)N_points;
      y[i] = rf.f(x[i]);
   }
   std::stringstream ss;
   ss << rf.function_name << " function, N_points = " << N_points;
   plt::title(ss.str());
   plt::plot(x, y, "r--");
   plt::xlim(rf.xmin, rf.xmax);
   plt::xlabel("x");
   plt::ylabel("f(x)");
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