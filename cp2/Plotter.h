#ifndef PLOTTER_H
#define PLOTTER_H

#include <string>
#include "../global.h"

class RootFinder;
class ODESolver;
class ChargeDistribution;

class Plotter {
public:
   void plot            (const RootFinder& rf);
   void plot            (const ODESolver& ode);
   void plot_differences(const ODESolver& ode);
   void plot            (const ChargeDistribution& cd);

   static void find_extrema(const std::vector<array>& y_arrays, const array& x,
                            double xmin, double xmax,
                            double* min, double* max);
   static void find_extrema(const std::vector<CoordsArray>& functions, const array& x,
                            double xmin, double xmax,
                            double* min, double* max);

   static void test(array x, std::vector<array> y,                    
                    std::vector<std::string> names,
                    double xmin, double xmax);
};

#endif