#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <numeric>
#include <sstream>
#include "../matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;
using namespace std;

double random_in_range(double min_val, double max_val) {
    return min_val + (max_val-min_val) * (double)rand()/RAND_MAX ;
}

double muon_pdf(double x, double tau) {
   return (1.0/tau) * exp(-x/tau);
}

std::pair<std::vector<double>, std::vector<double>> gaussian(double mu,
                                                             double sigma,
                                                             unsigned n_runs,
                                                             double xmin,
                                                             double xmax) {
   int n_points = 5000;
   std::vector<double> x(n_points), y(n_points);
   for (int i = 0; i < n_points; i++) {
      x[i] = xmin + i*(xmax-xmin)/(double)n_points;

      // n_runs * 0.045 because this gives (more or less) the peak of the histogram
      y[i] = n_runs * 0.045 * exp( -(x[i]-mu)*(x[i]-mu)/(2*sigma*sigma) );
   }
   return std::make_pair(x, y);
}

int main(int argc, char** argv) {
   srand(time(NULL));

   double tau = 2.2;//e-6;

   int n_tests_per_run = 1000;
   int n_runs          = (argc > 1) ? std::stoi(argv[1]) : 500;

   double xmin = 0.0;
   double xmax = 10.0*tau;
   double ymin = 0.0;
   double ymax = muon_pdf(xmin, tau);

   vector<double> average_lifetimes(n_runs);

   for (int i = 0; i < n_runs; i++) {
      printf("\rRunning: %.2f%%", i*100.0/(float)n_runs);
      vector<double> line_x(n_tests_per_run),   line_y(n_tests_per_run);   // pdf equation line
      vector<double> points_x(n_tests_per_run), points_y(n_tests_per_run); // generated points under the pdf curve
      double running_total = 0.0; // sum of all decay times that are considered in the simulated mean

      for (int j = 0; j < n_tests_per_run; j++) {
         line_x[j] = xmin + (xmax-xmin)*(double)j/(double)n_tests_per_run;
         line_y[j] = muon_pdf(line_x[j], tau);

         while (true) {
            double x  = random_in_range(xmin, xmax);
            double y1 = muon_pdf(x, tau);
            double y2 = random_in_range(ymin, ymax);
            if (y2 < y1) {
               points_x[j] = x;
               points_y[j] = y2;
               running_total += points_x[j];
               break;
            }
         }
      }
      // mean lifetime of the muon for this run
      average_lifetimes[i] = running_total / (double)n_tests_per_run;
   }

   // calculating mean and error in the simulated distribution
   double mu = std::accumulate(average_lifetimes.begin(), average_lifetimes.end(), 0.0) / average_lifetimes.size();
   double sigma = 0.0;
   for (auto& a : average_lifetimes)
      sigma += pow(a-mu, 2);
   sigma = sqrt(sigma / (average_lifetimes.size()-1));
   printf("\rExpected decay time  = %.4f microseconds\n", tau);
   printf("Simulated decay time = %.4f +- %.4f microseconds\n", mu, sigma);
   printf("                     = %.2f sigma from expected\n", abs(mu - tau)/sigma);

   int number_of_bins = 70;
   plt::hist(average_lifetimes, number_of_bins, "b", 0.3);
   double y_maximum = 1.1 * 0.05 * n_runs;
   plt::named_plot("Expected mean",  {tau, tau}, {0, y_maximum}, "r--");
   plt::named_plot("Simulated mean", {mu, mu},   {0, y_maximum}, "g--");

#if 1
   // lines to represent 1, 2, 3 sigma distance from the mean
   plt::plot({mu+1*sigma, mu+1*sigma}, {0, y_maximum}, "y-");
   plt::plot({mu-1*sigma, mu-1*sigma}, {0, y_maximum}, "y-");
   plt::plot({mu+2*sigma, mu+2*sigma}, {0, y_maximum}, "y-");
   plt::plot({mu-2*sigma, mu-2*sigma}, {0, y_maximum}, "y-");
   plt::plot({mu+3*sigma, mu+3*sigma}, {0, y_maximum}, "y-");
   plt::plot({mu-3*sigma, mu-3*sigma}, {0, y_maximum}, "y-");
#endif

   // overlaid expected gaussian distribution
   auto overlaid_gaussian = gaussian(mu, sigma, n_runs, mu-sigma*4, mu+sigma*4);
   plt::named_plot("Expected distribution", overlaid_gaussian.first, overlaid_gaussian.second, "b-");

   // set up plot area and bring it up
   std::stringstream title;
   title << n_runs << " runs of " << n_tests_per_run << " Monte-Carlo simulated muon decay times";
   plt::title(title.str());
   plt::xlabel("Decay time (microsecs)");
   plt::ylabel("Frequency");
   plt::legend();
   plt::ylim(0.0, y_maximum);
   plt::show();
}