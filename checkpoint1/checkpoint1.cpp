#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <numeric>
#include "../matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;
using namespace std;

double rand_range(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double muon_pdf(double x, double tau) {
   return (1.0/tau) * exp(-x/tau);
}

int main() {
   srand(time(NULL));  

   double tau = 2.2;//* pow(10, -6);

   int n_tests_per_run = 1000;
   int n_runs          = 500;

   double xmin = 0.0;
   double xmax = 10.0*tau;
   double ymin = 0.0;
   double ymax = muon_pdf(xmin, tau);

   vector<double> average_lifetimes(n_runs);

   for (int i = 0; i < n_runs; i++) {
      vector<double> line_x(n_tests_per_run), line_y(n_tests_per_run); // equation line
      vector<double> points_x(n_tests_per_run), points_y(n_tests_per_run); // generated points under the curve
      double running_total = 0.0;

      for (int j = 0; j < n_tests_per_run; j++) {
         line_x[j] = xmin + (xmax-xmin)*(double)j/(double)n_tests_per_run;
         line_y[j] = muon_pdf(line_x[j], tau);

         while (true) {
            double x = rand_range(xmin, xmax);
            double y1 = muon_pdf(x, tau);
            double y2 = rand_range(ymin, ymax);
            if (y2 < y1) {
               points_x[j] = x;
               points_y[j] = y2;
               running_total += points_x[j];
               break;
            }
         }
      }
      average_lifetimes[i] = running_total / (double)n_tests_per_run;
   }

   double mu = std::accumulate(average_lifetimes.begin(), average_lifetimes.end(), 0.0) / average_lifetimes.size();
   double sigma = 0;
   for (auto& a : average_lifetimes) sigma += pow(a-mu, 2);
   sigma = sqrt(sigma / (average_lifetimes.size()-1));
   cout << "mu_calculated = " << mu << " sigma = " << sigma << '\n';

   plt::hist(average_lifetimes, 50, "b", 0.5);
   double maximum = 40;
   plt::named_plot("expected mean",   {tau, tau}, {0, maximum}, "r--");
   plt::named_plot("calculated mean", {mu, mu},   {0, maximum}, "g--");
   plt::ylim(0.0, maximum);

   plt::title("Simulated muon decay times");
   plt::xlabel("Decay time (secs / 1e6)");
   plt::ylabel("Frequency");
   plt::legend();

   plt::show();
}