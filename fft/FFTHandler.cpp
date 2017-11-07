#include <iostream>
#include <math.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <fftw3.h>

#include "../global.h"
#include "FFTHandler.h"

double* FFTHandler::read_pgm_file(const char*   filename, 
                                        size_t* N_rows, 
                                        size_t* N_cols) {
   std::ifstream infile(filename, std::ios::binary);
   std::string inputLine;

   // First line : file version
   getline(infile, inputLine);

   // two commented lines
   getline(infile, inputLine);
   getline(infile, inputLine);

   // width, height, number of possible pixel values
   getline(infile, inputLine);
   std::stringstream ss(inputLine);
   ss >> *N_cols >> *N_rows;
   int max_bits;
   ss >> max_bits;
   printf("Rows = %zu Cols = %zu\n", *N_rows, *N_cols);
   int bit_depth = log2(max_bits + 1.0);
   printf("Bit depth = %d\n", bit_depth);

   // allocate 2D pixel array
   const size_t pixel_count = (*N_rows) * (*N_cols);
   double* pixels = new double[pixel_count];

   // pick out every character in the pgm file
   size_t i = 0;
   while (getline(infile, inputLine)) {
      for (const unsigned char pix : inputLine) {
         pixels[i++] = (double)pix; // shift to positive numbers
      }
      if (i <= pixel_count) {
         // At the end of each inputLine theres a new line character, which arent picked up without this line
         // Took me bloody ages to figure this out
         pixels[i++] = 10; // 10 = ascii code for '\n'
      }
   }

   infile.close();   
   return pixels; 
}

fftw_complex* FFTHandler::get_row(const double* pixels,
                                  const size_t  N_cols,
                                  const size_t  row_index) {
   fftw_complex* row_pixels = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N_cols);
   size_t j = row_index * N_cols;
   for (size_t i = 0; i < N_cols; i++) {                        
      row_pixels[i][0] = pixels[j++];
      row_pixels[i][1] = 0.0; // imaginary signal = 0 all across row
   }
   return row_pixels;
}

fftw_complex* FFTHandler::row_fft(      fftw_complex* row_pixels,
                                  const size_t        length,
                                  const bool          is_forward) {
   fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * length);
   int sign = is_forward ? FFTW_FORWARD : FFTW_BACKWARD;
   fftw_plan p = fftw_plan_dft_1d(length, row_pixels, out, sign, FFTW_ESTIMATE);

   fftw_execute(p);
   
   fftw_destroy_plan(p);

   if (!is_forward) {
      for (size_t i = 0; i < length; i++) {
         out[i][0] /= (double)length;
         out[i][1] /= (double)length;
      }
   }
   return out;
}

fftw_complex* FFTHandler::multiply(fftw_complex* a,
                                   fftw_complex* b,
                                   const size_t  N) {
   fftw_complex* result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
   for (size_t i = 0; i < N; i++) {
      const double A = a[i][0];
      const double B = a[i][1];
      const double C = b[i][0];
      const double D = b[i][1];
      result[i][0] = A*C - B*D;
      result[i][1] = B*C + A*D;
   }
   return result;
}

fftw_complex* FFTHandler::conjugate(fftw_complex* input,
                                    const size_t  N) {
   fftw_complex* output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
   for (size_t i = 0; i < N; i++) {
      output[i][0] = input[i][0];
      output[i][1] = input[i][1] * -1.0;
   }
   return output;
}

double FFTHandler::min_value(fftw_complex* a,
                             const size_t  N) {
   double minimum = 1e200;
   for (size_t i = 0; i < N; i++) {
      if (a[i][0] < minimum) minimum = a[i][0];
   }
   return minimum;
}

fftw_complex* FFTHandler::xcorr(fftw_complex* a,
                                fftw_complex* b,
                                const size_t  N) {
   fftw_complex* b_conj     = conjugate(b, N);
   fftw_complex* fft_a      = row_fft(a,      N, true);
   fftw_complex* fft_b      = row_fft(b_conj, N, true);
   fftw_complex* multiplied = multiply(fft_a, fft_b, N);
   fftw_complex* inverse    = row_fft(multiplied, N, false);

   double min_val = min_value(inverse, N);
   for (size_t i = 0; i < N; i++) {
      inverse[i][0] -= min_val;
   }

   fftw_free(b_conj);
   fftw_free(fft_a);
   fftw_free(fft_b);
   fftw_free(multiplied);
   return inverse;
}

void FFTHandler::copy(fftw_complex*  input,
                      const size_t   N,
                      fftw_complex** output) {
   for (size_t i = 0; i < N; i++) {
      (*output)[i][0] = input[i][0];
      (*output)[i][1] = input[i][1];
   }
}

double FFTHandler::sum_diffs_sq(fftw_complex* a,
                                const size_t  N,
                                double*       left,
                                double*       right) {
   double leftsum  = 0.0;
   double rightsum = 0.0;
   double sum      = 0.0;
   for (size_t i = 1; i < N; i++) {
      double diff = abs(a[i][0] - a[i-1][0]) + abs(a[i][1] - a[i-1][1]);
      diff = diff;
      sum += diff;
      if (i < N/2) leftsum  += diff;
      else         rightsum += diff;
   }
   *left  = leftsum;
   *right = rightsum;
   return sum;
}