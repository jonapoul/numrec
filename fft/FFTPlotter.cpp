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
   plt::clf();

   std::vector<double> re, im;
   fftw_complex_to_vectors(y, x.size(), &re, &im);

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
