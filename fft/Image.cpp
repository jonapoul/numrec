#include <iostream>
#include <math.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <limits>
#include <fftw3.h>

#include "../global.h"
#include "Image.h"

Image::Image(const char* file) { 
   read_pgm_file(file);
   extract_rows();
   BLANK_PIXEL = 1;
}

Image::~Image() {
   delete[] rows; 
}

bool Image::row_should_be_shifted(const size_t r, 
                                  const std::vector<int>& peaks) {
   // 0 shift => not shifting (obviously)
   if (peaks[r] == 0) {
      printf(" 0 ");
      return false;
   }
   if (rows[r].has_been_shifted) {
      printf(" 1 ");
      return false;
   }
   if (abs(peaks[r]) > rows[r].width/10.0) {
      printf(" 2 ");
      return false;
   }
   // if we have a random spike with no other responses nearby, assume its an error
   if (r != peaks.size()-1 && peaks[r+1] == 0 && peaks[r-1] == 0 && peaks[r] != 0) {
      printf(" 3 ");
      return false;
   }
   // if we have a mirrored peak between the two lines
   if (r != peaks.size()-1 && peaks[r] == -peaks[r+1] && peaks[r] != 0) {
      printf(" 4 ");
      return true;
   }
   // if we have a jump in peak magnitudes
   if (abs(peaks[r]) > 0 && peaks[r-1] == 0) {
      printf(" 5 ");
      return true;
   }
   // if we have an almost-mirrored peak between two lines
   if (peaks[r] * peaks[r-1] < 0 && peaks[r] != 0) {
      printf(" 6 ");
      return true;
   }
   // if the previous row is nonzero and too close to the current
   if (peaks[r-1] != 0 && abs(peaks[r]-peaks[r-1]) < 2) {
      printf(" 7 ");
      return false;
   }

   // if all else fails, shift it
   printf(" X ");
   return false;
}

bool Image::synchronise() {
   static int run_count = 1;
   printf("\n\nRun #%d...\n", run_count++);
   std::vector<int> peaks(height);
   std::vector<size_t> rows_to_be_shifted;

   /* find shift distances for each row */
   for (size_t r = 1; r < height; r++) {
      Row cross_corr = cross_correlate(rows[r], rows[r-1]);
      peaks[r] = peak(cross_corr);
   }
   for (size_t r = 1; r < height; r++) {
      bool test = row_should_be_shifted(r, peaks) && !is_in_array(r-1, rows_to_be_shifted);
      if (test) 
         rows_to_be_shifted.push_back(r);
      printf("row=%3zu peak at x=%4d prev=%s chosen=%s\n", r, peaks[r], (rows[r].has_been_shifted?"yes":"no "), (test?"yes":"no "));
   }

   if (rows_to_be_shifted.size() == 0) {
      return false; // our work here is done
   } else {
      printf("Rows to be shifted = ");
      for (auto x : rows_to_be_shifted) 
         printf("%zu ", x);
      printf("\n");
   }

   /* apply shifts to rows that actually need it */
   for (size_t r = 1; r < height; r++) {
      if (is_in_array(r, rows_to_be_shifted)) {
         const size_t shift_length = abs(peaks[r]);
         printf("shifting row %3zu ", r);

         Row right(rows[r]);
         Row left(rows[r]);
         right.shift(shift_length, RIGHT);
         left.shift(shift_length, LEFT);

         Row xcorr_right = cross_correlate(right, rows[r-1]);
         Row xcorr_left  = cross_correlate(left,  rows[r-1]);

         int peak_right  = peak(xcorr_right);
         int peak_left   = peak(xcorr_left);
         printf(" [left=%3d right=%3d] ", peak_left, peak_right);

         bool simple_case = (r < height-2) && 
                            (peaks[r] == -peaks[r+1]) && 
                            (peaks[r] != 0) &&
                            (peaks[r-1] == 0) && 
                            (peaks[r+2] == 0);
         
         Row::deallocate(rows[r]);
         if (simple_case) {
            printf("[simplecase]");
            if ( abs(peak_left) < abs(peak_right) ) {
               rows[r] = left;
               int new_peak = peak(cross_correlate(rows[r], rows[r-1]));
               printf("by %3d to the left,  peak moved from %3d to %3d, or %3d\n", peaks[r], peaks[r], new_peak, peak_left);
            } else {
               rows[r] = right;
               int new_peak = peak(cross_correlate(rows[r], rows[r-1]));
               printf("by %3d to the right, peak moved from %3d to %3d, or %3d\n", peaks[r], peaks[r], new_peak, peak_right);
            }
         } else {
            rows[r] = best_xcorr_in_range(shift_length+5, r);
         }

         // if (r == 226) {
         //    FFTPlotter::plot(rows[r], rows[r-1], "r vs r-1");
         //    FFTPlotter::plot(left, right, "left vs right");
         //    FFTPlotter::plot(xcorr_left, xcorr_right, "left vs right xcorr");
         // }
      } /* if is_in_array */
   } /* loop over rows */
   return true;
}

Row Image::best_xcorr_in_range(const size_t max_shift,
                               const size_t row_index) {
   /* initialise array of Row objects to calculate cross-corr peaks for */
   std::vector<Row> shifted(2*max_shift+1);
   for (int i = 0; i < (int)shifted.size(); i++) {
      shifted[i].initialise(this, row_index);
      const int direction = (i < (int)max_shift) ? LEFT : RIGHT;
      shifted[i].shift(abs((int)max_shift-i), direction);
   }

   /* calculate the peaks and determine which is closest to zero */
   std::vector<int> peaks(2*max_shift+1);
   size_t best_shift_index;   
   int best_peak_distance = std::numeric_limits<int>::max();
   for (size_t i = 0; i < peaks.size(); i++) {
      peaks[i] = peak(cross_correlate(shifted[i], rows[row_index-1]));
      printf("peaks[%3d]=%3d, max=%2d at shift=%d\n", (int)i-(int)max_shift, peaks[i], best_peak_distance, (int)best_shift_index-(int)max_shift);
      if (abs(peaks[i]) <= abs(best_peak_distance) &&
          abs((int)i-(int)max_shift) < abs((int)best_shift_index-(int)max_shift)) {
         best_peak_distance = peaks[i];
         best_shift_index = i;
      }
   }

   const int old_peak = peak(cross_correlate(rows[row_index], rows[row_index-1]));
   PRINT(old_peak);
   if (abs(best_peak_distance) > abs(old_peak)) {
      printf("          nowhere,    best calculated peak in range [%.0f,%.0f] was %d\n", 
             -1.0*max_shift, 1.0*max_shift, best_peak_distance);
      return rows[row_index];
   } else { 
      printf(" by %3d to the %s, best calculated peak in range [%.0f,%.0f] was %d\n", 
             (int)best_shift_index-(int)max_shift, (best_shift_index<max_shift?"left ":"right"), -1.*max_shift,1.*max_shift, best_peak_distance);
      return shifted[best_shift_index];
   }
}

void Image::read_pgm_file(const char* fn) {
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

void Image::extract_rows() {
   rows = new Row[height];
   for (size_t r = 0; r < height; r++) {
      rows[r].initialise(this, r);
   }
   // delete[] pixels; // remove this from memory since we dont need it any more
}

int Image::peak(const Row& r) const {
   double max_height = -1e200;
   int peak_position;
   std::vector<double> x(r.width);
   for (size_t i = 0; i < x.size(); i++) {
      x[i] = i - 0.5*r.width + (r.width%2==1?0.5:0);
   }
   for (size_t i = 0; i < r.width; i++) {
      if (r.magnitude(i) > max_height) {
         max_height = r.pixels[i][0];
         peak_position = ROUND(x[i]);
      }
   }
   return peak_position;
}

Row Image::cross_correlate(const Row& row1, 
                           const Row& row2) {
   /* getting the largest stretch of pixels thats covered by both row1 and row2 */
   const size_t first_index  = MAX(row1.starting_index, row2.starting_index);
   const size_t subrow_width = row1.overlapping_pixels_with(row2);
   const Row r1 = row1.subrow(first_index, subrow_width);
   const Row r2 = row2.subrow(first_index, subrow_width);

   if (r1.width != r2.width) {
      PRINT(subrow_width);
      r1.print_debug();
      r2.print_debug();
   }
   ASSERT( r1.width == r2.width );

   const Row fft1       = r1.fft();
   const Row fft2       = r2.fft();
   const Row multiplied = fft1 * fft2.conjugate();
   Row inversed         = multiplied.inverse_fft();

   /* shift the phase of the periodic function, so the peak is (ideally) near the middle of the curve */
   inversed.recentre();
   return inversed;
}

CImg<double> Image::to_cimg() {
   CImg<double> image(width, height, 1, 1);
   for (size_t r = 0; r < height; r++) {
      const size_t start_r = rows[r].starting_index;
      const size_t width_r = rows[r].width;
      size_t j = 0;
      for (size_t c = 0; c < this->width; c++) {
         if      (c < start_r)         image(c, r) = BLANK_PIXEL;
         else if (c < start_r+width_r) image(c, r) = rows[r].magnitude(j++);
         else                          image(c, r) = BLANK_PIXEL;

         if (image(c, r) > 255 || image(c, r) < 0) image(c, r) = BLANK_PIXEL;
      }
   }
   return image;
}