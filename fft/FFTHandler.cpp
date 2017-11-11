#include <iostream>
#include <math.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <fftw3.h>

#include "../global.h"
#include "FFTHandler.h"

FFTHandler::FFTHandler() { /*blank*/ }

FFTHandler::~FFTHandler() {
   delete[] pixels;
   delete[] rows; // this line crashes the whole thing when returning from main()???
}

void FFTHandler::initialise(const char* file) {
   read_pgm_file(file);
   extract_rows();
}

bool FFTHandler::row_should_be_shifted(const size_t r, 
                                       const std::vector<int>& peaks) {
   if (r > peaks.size()-2) 
      return false;
   if (peaks[r] == -peaks[r+1] && peaks[r] != 0 && peaks[r+1] != 0) 
      return true;
   if (abs(peaks[r] - peaks[r-1]) > 100) 
      return true;
   return false;
}

void FFTHandler::synchronise() {
   static int run_count = 1;
   printf("\n\nRun #%d...\n", run_count++);
   std::vector<int> peaks(height);
   std::vector<size_t> rows_to_be_shifted;

   /* find shift distances for each row */
   for (size_t r = 1; r < height; r++) {
      printf("checking peak for row=%zu\n", r);
      Row cross_corr = xcorr(rows[r], rows[r-1]);
      peaks[r] = peak(cross_corr);
   }
   for (size_t r = 1; r < height; r++) {
      bool test = row_should_be_shifted(r, peaks) && !is_in_array(r-1, rows_to_be_shifted);
      if (test) 
         rows_to_be_shifted.push_back(r);
      printf("row=%3zu peak at x=%4d shifted=%d\n", r, peaks[r], (int)test);
   }

   for (auto x : rows_to_be_shifted) printf("%zu ", x);
   printf("\n");

   /* apply shifts to rows that actually need it */
   for (size_t r = 1; r < height; r++) {
      if (is_in_array(r, rows_to_be_shifted)) {
         const size_t shift_length = abs(peaks[r]);
         printf("shifting row %3zu by %3d", r, peaks[r]);

         Row right(rows[r]);
         Row left(rows[r]);
         right.shift(shift_length, RIGHT);
         left.shift(shift_length, LEFT);

         Row subrow_prev_right = rows[r-1].subrow(shift_length, rows[r-1].width-shift_length);
         Row subrow_prev_left  = rows[r-1].subrow(0,            rows[r-1].width-shift_length);
         Row xcorr_right = xcorr(right, subrow_prev_right);
         Row xcorr_left  = xcorr(left,  subrow_prev_left);

         // FFTPlotter::plot(right, subrow_prev_right, "right vs prev");
         // FFTPlotter::plot(left,  subrow_prev_left,  "left vs prev");
         // FFTPlotter::plot(xcorr_left, xcorr_right, "cross correlations");

         int peak_right  = peak(xcorr_right);
         int peak_left   = peak(xcorr_left);

         fftw_free(rows[r].pixels);
         if ( abs(peak_left) < abs(peak_right) ) {
            rows[r].shifted_distance = shift_length;
            rows[r].starting_index = 0;
            rows[r].width = left.width;
            rows[r].pixels = allocate(left.width);
            for (size_t i = 0; i < left.width; i++) {
               rows[r].pixels[i][0] = left.pixels[i][0];
               rows[r].pixels[i][1] = left.pixels[i][1];
            }
            printf(" to the left,  peak moved from %d to %d\n", peaks[r], peak_left);
         } else {
            rows[r].shifted_distance = shift_length;
            rows[r].starting_index = shift_length;
            rows[r].width = right.width;
            rows[r].pixels = allocate(right.width);
            for (size_t i = 0; i < right.width; i++) {
               rows[r].pixels[i][0] = right.pixels[i][0];
               rows[r].pixels[i][1] = right.pixels[i][1];
            }
            printf(" to the right, peak moved from %d to %d\n", peaks[r], peak_right);
         }
      }
   }
}

void FFTHandler::read_pgm_file(const char* fn) {
   filename = fn;
   const size_t pos = filename.find_last_of(".");
   std::string extension = filename.substr(pos+1);
   ASSERT( extension == "pgm" );

   std::ifstream infile(filename, std::ios::binary);
   ASSERT( infile.is_open() );

   std::string inputLine;
   getline(infile, inputLine);   // file version
   getline(infile, inputLine);   // comment
   getline(infile, inputLine);   // comment
   getline(infile, inputLine);   // width, height, num possible pixel values
   std::stringstream ss(inputLine);
   ss >> width >> height;
   int max_bits;
   ss >> max_bits;
   int bit_depth = log2(max_bits + 1.0);
   printf("height    = %zu\nwidth     = %zu\nbit depth = %d\n", height, width, bit_depth);

   // allocate 1D array to hold every pixel in the image
   const size_t pixel_count = height * width;
   pixels = new double[pixel_count];

   // pick out every plaintext ascii character in the pgm file
   size_t i = 0;
   while (getline(infile, inputLine)) {
      for (const unsigned char pix : inputLine) {
         pixels[i++] = (double)pix;
      }
      if (i < pixel_count) {
         // At the end of each inputLine theres a new line character, which isn't picked up without this bit
         // Took me bloody ages to figure this out
         pixels[i++] = 10; // 10 = ascii code for '\n'
      }
   }
   infile.close();
}

void FFTHandler::extract_rows() {
   rows = new Row[height];
   for (size_t r = 0; r < height; r++) {
      rows[r].initialise(this, r);
   }
}

int FFTHandler::peak(const Row& r) const {
   double max_height = -1e200;
   int peak_position;
   std::vector<double> x(r.width);
   for (size_t i = 0; i < x.size(); i++) {
      x[i] = -0.5*r.width + (double)i;
   }
   for (size_t i = 0; i < r.width; i++) {
      if (magnitude(r.pixels[i]) > max_height) {
         max_height = r.pixels[i][0];
         peak_position = x[i];
      }
   }
   return peak_position;
}

Row FFTHandler::xcorr(const Row& row1, 
                      const Row& row2) {
   ASSERT( row1.width == row2.width );

   Row fft1       = row1.fft();
   Row fft2       = row2.fft();
   Row multiplied = fft1 * fft2.conjugate();
   Row inversed   = multiplied.inverse_fft();

   /* shift the phase of the periodic function, so the peak is (ideally) near the middle of the curve */
   inversed.recentre();
   return inversed;
   return fft1;
}

CImg<double> FFTHandler::to_cimg() {
   const double BLANK_PIXEL = 0.0;
   CImg<double> image(width, height, 1, 1);

   for (size_t r = 0; r < height; r++) {
      for (size_t c = 0; c < rows[r].starting_index; c++) {
         image(c, r) = BLANK_PIXEL;
      }
      for (size_t c = rows[r].starting_index; c < rows[r].width; c++) {
         image(c, r) = magnitude(rows[r].pixels[c]);
      }
      for (size_t c = rows[r].width; c < this->width; c++) {
         image(c, r) = BLANK_PIXEL;
      }
   }
   return image;
}

inline double FFTHandler::magnitude(fftw_complex a) const {
   return sqrt(a[0]*a[0] + a[1]*a[1]);
}