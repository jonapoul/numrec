#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <numeric>
#include <sstream>

#include "functions.h"
#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp;

int main(int argc, char** argv) {
   srand(time(NULL));

   double tau = 2.2;//e-6;

   int n_tests_per_run = 1000;
   int n_runs          = (argc > 1) ? std::stoi(argv[1]) : 500;

   // limits of the monte-carlo number generation from muon_pdf()
   double xmin = 0.0;
   double xmax = 10.0*tau;
   double ymin = 0.0;
   double ymax = muon_pdf(xmin, tau);

   std::vector<double> average_lifetimes(n_runs);
   std::vector<double> single_run_decay_times(n_tests_per_run);
   std::ofstream exponential_decay_times_file;
   exponential_decay_times_file.open("exponential_decay_times.txt");

   for (int i = 0; i < n_runs; i++) {
      // prints current running percentage
      printf("\rRunning: %.2f%%", i*100.0/(float)n_runs);

      std::vector<double> points_x(n_tests_per_run), points_y(n_tests_per_run); // generated points under the pdf
      double running_total = 0.0; // sum of all decay times that are considered in the simulated mean

      for (int j = 0; j < n_tests_per_run; j++) {
         bool has_found_point_under_line = false;
         while (!has_found_point_under_line) {
            double x           = random_in_range(xmin, xmax);
            double y_pdf       = muon_pdf(x, tau);
            double y_generated = random_in_range(ymin, ymax);
            // if the generated point is under the pdf line
            if (y_generated < y_pdf) {
               points_x[j] = x; // add the x/y coords to the arrays
               points_y[j] = y_generated;
               running_total += x; // used to calculate mean decay time at the end
               has_found_point_under_line = true;
            }
            // otherwise just try again
         }
         if (i == 0) {
            single_run_decay_times[j] = points_x[j];
            exponential_decay_times_file << single_run_decay_times[j] << '\n';
         }
      }
      // mean lifetime of the muon for this run
      average_lifetimes[i] = running_total / (double)n_tests_per_run;
   }
   exponential_decay_times_file.close();




   int number_of_bins = 70;
   {
      // mean of the plotted decay curve
      double mu = average_lifetimes[0];
      printf("\rExponential Decay Curve:        \n");
      printf("\tExpected decay time  = %.4f microseconds\n", tau);
      printf("\tSimulated decay time = %.4f microseconds\n", mu);
      printf("\tDifference           = %.4f microseconds\n\n", abs(mu-tau));

      // plot of exponential decay
      plt::hist(single_run_decay_times, number_of_bins, "b", 0.3);

      double y_maximum = 0.12 * n_tests_per_run;
      plt::named_plot("Expected mean", {tau, tau}, {0, y_maximum}, "r--");
      plt::named_plot("Simulated mean", {mu, mu}, {0, y_maximum}, "g--");

      // overlaid exponential decay curve
      CoordsArray exponential_decay = exp_decay(tau, n_tests_per_run, xmin, xmax);
      plt::named_plot("Expected distribution", exponential_decay.first, exponential_decay.second, "b-");

      std::stringstream title;
      title << n_tests_per_run << " Monte-Carlo simulated muon decay times";
      plt::title(title.str());
      plt::xlim(xmin, xmax);
      plt::ylim(0.0, y_maximum);
      plt::xlabel("Decay time (microsecs)");
      plt::ylabel("Frequency");
      plt::legend();
      plt::show();
   }

   {
      // calculating mean and error in the simulated distribution
      double mu = std::accumulate(average_lifetimes.begin(), average_lifetimes.end(), 0.0) / average_lifetimes.size();
      double sigma = 0.0;
      for (auto& a : average_lifetimes)
         sigma += pow(a-mu, 2);
      sigma = sqrt(sigma / (average_lifetimes.size()-1));
      printf("Distribution of all exponential decay curves:\n");
      printf("\tExpected decay time  = %.4f microseconds\n", tau);
      printf("\tSimulated decay time = %.4f +- %.4f microseconds\n", mu, sigma);
      printf("\t                     = %.2f sigma from expected\n", abs(mu - tau)/sigma);

      // plot of average decay time probability
      plt::hist(average_lifetimes, number_of_bins, "b", 0.3);

      // two dashed lines indicating the expected/simulated mean decay times
      double y_maximum = 1.1 * 0.05 * n_runs;
      plt::named_plot("Expected mean",  {tau, tau}, {0, y_maximum}, "r--");
      plt::named_plot("Simulated mean", {mu, mu},   {0, y_maximum}, "g--");

      // overlaid expected gaussian distribution
      CoordsArray overlaid_gaussian = gaussian(mu, sigma, n_runs, mu-sigma*4, mu+sigma*4);
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
}