#include <numeric>
#include <algorithm>

#include "../global.h"
#include "FFTHandler.h"
#include "FFTPlotter.h"

int main() {
   FFTHandler handler; // class to do the business behind the scenes
   FFTPlotter plotter;
   const char* filename = "fft/images/desync1.pgm";

   size_t N_rows, N_cols;
   double* pixels = handler.read_pgm_file(filename, &N_rows, &N_cols);

   std::vector<double> diffs;
   std::vector<double> x = generate_smooth_x_values(0.0, (double)N_cols, 1.0);
   std::vector<size_t> estimated_lines = {5, 10, 16, 24, 28, 33, 34, 36, 49, 51, 59, 71, 78, 81, 84, 90, 97, 98, 113, 118, 128, 198, 208, 227, 230, 244, 247, 255, 274, 288, 290, 314, 316, 344, 360, 361, 362, 366, 392, 415, 434, 435, 443, 453, 465, 468, 469, 472, 485, 498, 500, 507};

   for (size_t r = 1; r < N_rows; r++) { // assuming no shifts in the zeroeth row
      fftw_complex* row        = handler.get_row(pixels, N_cols, r);
      fftw_complex* row_prev   = handler.get_row(pixels, N_cols, r-1);
      fftw_complex* cross_corr = handler.xcorr(row, row_prev, N_cols);

      double left, right;
      double sumsq = handler.sum_diffs_sq(cross_corr, N_cols, &left, &right);
      diffs.push_back(sumsq);
      double lean = 100.0 * (1.0 - left/right);
      printf("Rows %03zu-%03zu: sumsq=%.2e left=%.2e right=%.2e lean=%6.2f%% pred=%d\n", 
             r-1, r, sumsq, left, right, lean, is_in_array(r,estimated_lines));

      //plotter.plot(x, {row, row_prev}, r, r-1);
      plotter.plot(x, {cross_corr}, r, r-1);

      fftw_free(row);
      fftw_free(row_prev);
      fftw_free(cross_corr);
   }

   x.erase(x.begin());
   plotter.plot(x, estimated_lines, diffs);

   delete[] pixels;
   return 0;
}
