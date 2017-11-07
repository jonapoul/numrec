#include "../global.h"
#include "FFTHandler.h"
#include "FFTPlotter.h"

int main() {
   FFTHandler handler("fft/images/desync1.pgm");
   FFTPlotter plotter;

   handler.read_pgm_file();
   handler.extract_rows();
   std::vector<double> x = increment_x_values(handler.width/-2.0, handler.width);

   for (size_t r = 1; r < handler.width; r++) {
      fftw_complex* cross_corr = handler.xcorr(r);
      double peak_position = handler.peak(cross_corr, x);
      printf("row=%3zu Peak=%.0f\n", r, peak_position);

      plotter.plot(x, cross_corr, r, r-1);
      fftw_free(cross_corr);
   }

   return 0;
}
