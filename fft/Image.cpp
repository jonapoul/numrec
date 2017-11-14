#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>
#include <CImg/CImg.h>
using namespace cimg_library;

#include "../global.h"
#include "Image.h"

Image::Image(const std::string& file) { 
   read_pgm_file(file);
   extract_rows();
   BLANK_PIXEL = 1;
}

Image::~Image() {
   delete[] rows;
   delete[] pixels; 
}

bool Image::row_should_be_shifted(const size_t r, 
                                  const std::vector<int>& peaks) {
   // 0 shift => not shifting (obviously)
   if (peaks[r] == 0) {
      if (PRINT_DEBUG) printf(" 0 ");
      return false;
   }
   // shifting too far, probably a mistake somewhere
   if (abs(peaks[r]) > rows[r].width/10.0) {
      if (PRINT_DEBUG) printf(" 1 ");
      return false;
   }
   if (abs(peaks[r-1]) > rows[r].width/10.0) {
      if (PRINT_DEBUG) printf(" 2 ");
      return false;
   }
   if (rows[r].has_been_shifted) {
      if (PRINT_DEBUG) printf(" 3 ");
      return false;
   }
   if (r > 2 && abs(peaks[r-2]) > 5 && abs(peaks[r-1]) > 5 && abs(peaks[r]) > 5 && 
       peaks[r]*peaks[r-1] < 0 && peaks[r]*peaks[r-2] < 0 ) {
      if (PRINT_DEBUG) printf(" 4 ");
      return false;
   }
   // if we have a mirrored peak between the two lines
   if (r < peaks.size()-1 && peaks[r] == -peaks[r+1] && peaks[r] != 0) {
      if (PRINT_DEBUG) printf(" 5 ");
      return true;
   }
   // if we have a jump in peak magnitudes
   if (abs(peaks[r]) > 1 && peaks[r-1] == 0) {
      if (PRINT_DEBUG) printf(" 6 ");
      return true;
   }
   // if we have an almost-mirrored peak between two lines
   if (peaks[r]*peaks[r-1] < 0 && peaks[r] != 0 && abs(peaks[r]-peaks[r-1]) < 5) {
      if (PRINT_DEBUG) printf(" 7 ");
      return true;
   }
   // if the previous row is nonzero and too close to the current
   if (peaks[r-1] != 0 && abs(peaks[r]-peaks[r-1]) < 2) {
      if (PRINT_DEBUG) printf(" 8 ");
      return false;
   }
   /* detect annoying blocks of two, e.g. lines 147-9 in image 3 */
   if (r < peaks.size()-1 /*&& peaks[r-1] != 0 && peaks[r-2] != 0*/) {
      const int sum = peaks[r] + peaks[r-1] + peaks[r-2];
      if (abs(sum) <= 3) {
         if (PRINT_DEBUG) printf(" 9 ");
         return false;
      }
   }
   if (abs(peaks[r] - peaks[r-1]) > 5) {
      if (PRINT_DEBUG) printf("10 ");
      return true;
   }

   // if all else fails, shift it just in case
   if (PRINT_DEBUG) printf(" X ");
   return true;
}

void Image::synchronise() {
   std::vector<int> peaks(height);
   std::vector<size_t> rows_to_be_shifted;

   /* find shift distances for each row */
   for (size_t r = 1; r < height; r++) {
      peaks[r] = peak(cross_correlate(rows[r], rows[r-1]));
   }
   for (size_t r = 1; r < height; r++) {
      bool shift_it = row_should_be_shifted(r, peaks) && !is_in_array(r-1, rows_to_be_shifted);
      if (shift_it) rows_to_be_shifted.push_back(r);
      if (PRINT_DEBUG) printf("row=%3zu peak at x=%4d prev=%s chosen=%s\n", r, peaks[r], (rows[r].has_been_shifted?"yes":"no "), (shift_it?"yes":"no "));
   }

   printf("Rows shifted = ");
   for (auto x : rows_to_be_shifted) 
      printf("%zu ", x);
   printf("\n");

   /* apply shifts to rows that actually need it */
   for (size_t r = 1; r < height; r++) {     
      if (is_in_array(r, rows_to_be_shifted)) {
         if (is_in_array(r-1, rows_to_be_shifted)) {
            /* update the shift distance based on any previous shifts */
            peaks[r] = peak(cross_correlate(rows[r], rows[r-1]));
         }
         if (PRINT_DEBUG) {
            printf("shifting row %3zu ", r);
         }
         rows[r].shift(peaks[r]);
         if (PRINT_DEBUG) {
            printf(" by %3d, new peak = %3d ", peaks[r], peak(cross_correlate(rows[r], rows[r-1])));
            if (r < height-1) { 
               printf(", next peak changed from %3d to ", peaks[r+1]);
               printf("%3d\n", peak(cross_correlate(rows[r+1], rows[r])));
            } else {
               printf("\n");
            }
         }
      } /* if is_in_array */
   } /* loop over rows */
}

void Image::read_pgm_file(const std::string& fn) {
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
   getline(infile, inputLine);   // width, height, bit depth
   std::stringstream ss(inputLine);
   ss >> width >> height;        // ignore bit depth, we dont need it

   // allocate 1D array to hold every pixel in the image
   const size_t pixel_count = height * width;
   pixels = new double[pixel_count];

   // pick out every plaintext ascii character in the pgm file
   size_t i = 0;
   while (getline(infile, inputLine)) {
      for (const unsigned char pix : inputLine)
         pixels[i++] = (double)pix;
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

   ASSERT( r1.width == r2.width );

   const Row fft1       = r1.fft();
   const Row fft2       = r2.fft();
   const Row multiplied = fft1 * fft2.conjugate();
   Row inversed         = multiplied.inverse_fft();

   /* shift the phase of the periodic function, so the peak is (ideally) at the middle of the curve */
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