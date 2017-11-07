#include <iostream>
#include <math.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <fftw3.h>

#include "../global.h"
#include "FFTHandler.h"

FFTHandler::FFTHandler(const char* fn) : filename(fn) { 
   estimated_lines = {
      5, 10, 16, 24, 28, 33, 34, 36, 49, 51, 59, 71, 78, 81, 84, 90, 97, 98, 113, 118, 128, 198, 
      208, 227, 230, 244, 247, 255, 274, 288, 290, 314, 316, 344, 360, 361, 362, 366, 392, 415, 
      434, 435, 443, 453, 465, 468, 469, 472, 485, 498, 500, 507
   };
}

FFTHandler::~FFTHandler() {
   delete[] pixels;
   for (size_t r = 0; r < height; r++) { 
      fftw_free(rows[r]);
   }
   delete[] rows;
}

fftw_complex* FFTHandler::allocate(const size_t N) {
   fftw_complex* output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
   return output;
}

void FFTHandler::read_pgm_file() {
   size_t pos = filename.find_last_of(".");
   std::string extension = filename.substr(pos+1);
   if (extension != "pgm") {
      printf("%s needs to be a .pgm file!\n", filename.c_str());
      exit(1);
   }

   std::ifstream infile(filename, std::ios::binary);
   std::string inputLine;
   getline(infile, inputLine);   // file version
   getline(infile, inputLine);   // two commented lines
   getline(infile, inputLine);
   
   getline(infile, inputLine);   // width, height, range of pixel values
   std::stringstream ss(inputLine);
   ss >> width >> height;
   int max_bits;
   ss >> max_bits;
   int bit_depth = log2(max_bits + 1.0);
   printf("height    = %zu\nwidth     = %zu\nbit depth = %d\n", height, width, bit_depth);

   // allocate 2D pixel array
   const size_t pixel_count = height * width;
   pixels = new double[pixel_count];

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
}

void FFTHandler::extract_rows() {
   rows    = new fftw_complex*[height];
   size_t i = 0;
   for (size_t r = 0; r < height; r++) {
      rows[r]    = allocate(width);
      for (size_t c = 0; c < width; c++) {
         rows[r][c][0] = pixels[i++];
         rows[r][c][1] = 0.0;
      }
   }
}

fftw_complex* FFTHandler::fft_row(fftw_complex* row,
                                  const int fft_direction) {
   fftw_complex* transformed = allocate(width);
   fftw_plan p = fftw_plan_dft_1d(width, row, transformed, fft_direction, FFTW_ESTIMATE);
   fftw_execute(p);
   fftw_destroy_plan(p);
   return transformed;
}

void FFTHandler::conjugate(fftw_complex* row) {
   for (size_t i = 0; i < width; i++) {
      row[i][1] *= -1.0;
   }
}

fftw_complex* FFTHandler::multiply(fftw_complex* a,
                                   fftw_complex* b) {
   fftw_complex* multiplied = allocate(width);
   for (size_t i = 0; i < width; i++) {
      const double A = a[i][0];
      const double B = a[i][1];
      const double C = b[i][0];
      const double D = b[i][1];
      multiplied[i][0] = A*C - B*D;
      multiplied[i][1] = B*C + A*D;
   }
   return multiplied;
}

void FFTHandler::centre_on_0(fftw_complex* a) {
   fftw_complex* temp = allocate(width);
   for (size_t i = 0; i < width/2; i++) {
      temp[i][0] = a[i + width/2][0];
      temp[i][1] = a[i + width/2][1];
   }
   for (size_t i = width/2; i < width; i++) {
      temp[i][0] = a[i - width/2][0];
      temp[i][1] = a[i - width/2][1];
   }
   for (size_t i = 0; i < width; i++) {
      a[i][0] = temp[i][0];
      a[i][1] = temp[i][1];
   }
   fftw_free(temp);
}

double FFTHandler::peak(fftw_complex* a, 
                        const std::vector<double>& x) {
   double max_height = -1e200;
   double peak_position;
   for (size_t i = 0; i < width; i++) {
      if (a[i][0] > max_height) {
         max_height = a[i][0];
         peak_position = x[i];
      }
   }
   return peak_position;
}

fftw_complex* FFTHandler::xcorr(const size_t row_index) {
   EXIT_IF_FALSE(row_index > 0);
   auto ft_curr = fft_row(rows[row_index],   FFTW_FORWARD);
   auto ft_prev = fft_row(rows[row_index-1], FFTW_FORWARD);
   conjugate(ft_prev);
   fftw_complex* multiplied = multiply(ft_curr, ft_prev);

   fftw_complex* inversed = fft_row(multiplied, FFTW_BACKWARD);
   for (size_t i = 0; i < width; i++) {
      inversed[i][0] /= (double)width;
      inversed[i][1] /= (double)width;
   }

   centre_on_0(inversed); // shifting the phase of the periodic function, so the peak is somewhere near the middle

   fftw_free(multiplied);
   fftw_free(ft_curr);
   fftw_free(ft_prev);

   return inversed;
}