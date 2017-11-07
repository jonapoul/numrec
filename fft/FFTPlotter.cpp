#include <sstream>
#include <fstream>
#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

#include "../global.h"
#include "FFTPlotter.h"

void FFTPlotter::plot(const std::vector<double>&        x, 
                      const std::vector<fftw_complex*>& y,
                      const size_t                      row,
                      const size_t                      row_prev) {
   plt::clf();
   for (size_t i = 0; i < y.size(); i++) {
      std::vector<double> re, im;
      fftw_complex_to_vectors(y[i], x.size(), &re, &im);

      char rebuf[100], imbuf[100];
      snprintf(rebuf, 100, "%zu real", i);
      std::string format = " -";
      format[0] = colours[i % colours.length()];
      plt::named_plot(rebuf, x, re, format);

      snprintf(imbuf, 100, "%zu imaginary", i);
      format = format + "-";
      format[0] = colours[(i+1) % colours.length()];
      //plt::named_plot(imbuf, x, im, format);
   }
   plt::grid(true);

   if (y[0][0][0] < 255) // if this is a straight pixel values plot
      plt::ylim(0.0, 255.0);

   char titlebuf[100];
   snprintf(titlebuf, 100, "row %zu to %zu", row, row_prev);
   plt::tight_layout();
   plt::title(titlebuf);
   plt::legend();
   plt::show();
}

void FFTPlotter::plot(const std::vector<double>& x, 
                      const std::vector<size_t>& estimated_lines, 
                      const std::vector<double>& diffs) {
   plt::clf();
   std::vector<double> lines_binary(diffs.size());
   for (size_t i = 0; i < lines_binary.size(); i++) {
      if (is_in_array(x[i], estimated_lines)) lines_binary[i] = diffs[i];
      else                                    lines_binary[i] =        0;
   }
   printf("%zu %zu %zu\n", x.size(), lines_binary.size(), diffs.size());
   plt::named_plot("predictions",          x, lines_binary, "g-");
   plt::named_plot("sum of diffs squared", x, diffs,        "r-");
   plt::grid(true);
   plt::title("Measured diffs agains predictions");
   plt::tight_layout();
   plt::legend();
   plt::show();
}
