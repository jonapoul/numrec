#include <sstream>
#include <fstream>
#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

#include "../global.h"
#include "FFTPlotter.h"

void FFTPlotter::plot(const std::vector<double>& x, 
                      fftw_complex* y,
                      const size_t row,
                      const size_t row_prev) {

   std::vector<double> re, im;
   fftw_complex_to_vectors(y, x.size(), &re, &im);
   
   plt::clf();
   plt::named_plot("real", x, re, "r-");
   // plt::named_plot("imaginary", x, im, "g-");

   char titlebuf[100];
   snprintf(titlebuf, 100, "%zu to %zu", row_prev, row);
   plt::tight_layout();
   plt::title(titlebuf);
   plt::grid(true);
   // plt::legend();
   plt::show();
}

void FFTPlotter::plot(const Row& r1,
                      const Row& r2,
                      const std::string& titlestr) {
   std::vector<double> re1, re2, im1, im2;
   fftw_complex_to_vectors(r1.pixels, r1.width, &re1, &im1);
   fftw_complex_to_vectors(r2.pixels, r2.width, &re2, &im2);
   std::vector<double> x1(re1.size()), x2(re2.size());
   for (size_t i = 0; i < r1.width; i++) {
      x1[i] = r1.starting_index + i - 0.5*r1.width;
   }
   for (size_t i = 0; i < r2.width; i++) {
      x2[i] = r2.starting_index + i - 0.5*r2.width;
   }

   plt::clf();
   plt::subplot(2, 1, 1);
   plt::named_plot("left shift", x1, re1, "r-");
   plt::tight_layout();
   plt::grid(true);
   plt::legend();
   plt::title(titlestr);

   plt::subplot(2, 1, 2);
   plt::named_plot("right shift", x2, re2, "g-");
   plt::tight_layout();
   plt::grid(true);
   plt::legend();
   plt::title(titlestr);

   plt::show();
}

void FFTPlotter::plot(const Row& r) {
   std::vector<double> re, im;
   fftw_complex_to_vectors(r.pixels, r.width, &re, &im);
   plt::clf();
   std::vector<double> x(r.width);
   for (size_t i = 0; i < r.width; i++) {
      x[i] = -0.5*r.width + i + r.starting_index;
   }
   plt::plot(x, re, "r-");
   plt::tight_layout();
   plt::title("title");
   plt::grid(true);
   // plt::legend();

   char titlebuf[100];
   snprintf(titlebuf, 100, "%zu to %zu", r.row_index, r.row_index-1);
   plt::title(titlebuf);
   plt::show();
}