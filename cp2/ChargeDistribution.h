#ifndef CHARGEDISTRIBUTION_H
#define CHARGEDISTRIBUTION_H

#include <vector>
#include <math.h>
#include <string>
using std::vector;

class ChargeDistribution {
public:
   friend class Plotter;
   ChargeDistribution();
   void plot(const std::string& title, bool show_plot = true);

private:
   double x0;
   double x1;
   double x2;
   double x3;
   double x4;
   double k;

   double shape(double x0, double x1, double x2) const;
   double evaluate(double x) const;
   void get_XY_values(double start, double end, int N_points, vector<double>* x, vector<double>* y) const;

};

#endif