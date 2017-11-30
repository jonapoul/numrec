#include <sstream>
#include <fstream>
#include "../matplotlibcpp.h"
namespace plt = matplotlibcpp; 

#include "../global.h"
#include "Row.h"
#include "FFTPlotter.h"

void FFTPlotter::plot(const Row& r1,
                      const Row& r2,
                      const std::string& titlestr) {
   std::vector<double> re1, re2, im1, im2;
   fftw_complex_to_vectors(r1.pixels, r1.width, &re1, &im1);
   fftw_complex_to_vectors(r2.pixels, r2.width, &re2, &im2);
   std::vector<double> x1(re1.size()), x2(re2.size());
   for (size_t i = 0; i < r1.width; i++) {
      x1[i] = i - 0.5*r1.width + (r1.width%2==1?0.5:0) + r1.starting_index;
   }
   for (size_t i = 0; i < r2.width; i++) {
      x2[i] = i - 0.5*r2.width + (r2.width%2==1?0.5:0) + r2.starting_index;
   }

   plt::named_plot("r1", x1, re1, "r-");
   plt::named_plot("r2", x2, re2, "g-");
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
      x[i] = -0.5*r.width + i + (r.width%2==1?0.5:0);
   }
   plt::plot(x, re, "r-");
   //plt::tight_layout();
   plt::grid(true);
   plt::xlabel("x");
   plt::ylabel("h(x)");
   plt::show();
}