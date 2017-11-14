#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>

#include "../global.h"
#include "Image.h"

Image::Image(const std::string& file)
         : filename(file), BLANK_PIXEL(1) {
   /* check that image filename is valid */ 
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
   ss >> this->width >> this->height;  // ignore bit depth, we dont need it

   /* allocate temporary 1D array to hold every pixel in the image */
   const size_t pixel_count = this->height * this->width;
   this->pixels = new double[pixel_count];

   /* pick out every plaintext ascii character in the pgm file */
   size_t i = 0;
   while (getline(infile, inputLine)) {
      for (const unsigned char pix : inputLine)
         this->pixels[i++] = (double)pix;
      if (i < pixel_count) {
         /* At the end of each inputLine theres a new line character, which 
            isn't picked up without this bit. Took me ages to figure this out */
         this->pixels[i++] = 10; // 10 = ascii code for '\n'
      }
   }
   infile.close();

   /* allocate and initialise each Row object by pulling from this->pixels in 
      the constructor */
   this->rows = new Row[this->height];
   for (size_t r = 0; r < this->height; r++) {
      this->rows[r].initialise(this, r);
   }
   delete[] this->pixels; /* we don't need this anymore */
}

Image::~Image() {
   delete[] this->rows;
}

/* These are the rules I put together to decide whether a row needs to be 
   shifted or not. They depend mostly on the precalculated x-positions of the 
   cross-correlation peaks. The debug printing of integers is printed to the 
   start of each line to indicate which option was chosen, just to help follow 
   the flow of logic. I realise that some of these are probably redundant but 
   I didn't want to risk breaking it
   
      r = row index that we're looking at
      peaks = array of x-coordinates of each row's cross-correlation
    */
bool Image::row_should_be_shifted(const size_t r, 
                                  const std::vector<int>& peaks) {
   /* 0 shift => not shifting (obviously) */
   if (peaks[r] == 0) {
      if (PRINT_DEBUG) printf(" 0 ");
      return false;
   }
   /* shifting too far, probably a mistake somewhere */
   if (abs(peaks[r]) > rows[r].width/10.0) {
      if (PRINT_DEBUG) printf(" 1 ");
      return false;
   }
   /* ignore any rows that have already had shifts applied */
   if (rows[r].has_been_shifted) {
      if (PRINT_DEBUG) printf(" 2 ");
      return false;
   }
   /* made to detect blocks of 2 shifted rows immediately before this row */
   if (                  r > 2 && 
       abs(peaks[r-2])     > 5 && 
       abs(peaks[r-1])     > 5 && 
       abs(peaks[r])       > 5 && 
       peaks[r]*peaks[r-1] < 0 && 
       peaks[r]*peaks[r-2] < 0 ) {
      if (PRINT_DEBUG) printf(" 3 ");
      return false;
   }
   /* to detect a single shifted line on its own */
   if (r < peaks.size()-1 && 
       peaks[r] == -peaks[r+1] && 
       peaks[r] != 0) {
      if (PRINT_DEBUG) printf(" 4 ");
      return true;
   }
   /* if the previous row is nonzero and too close to the current.
      Mostly pops up in the diagonal trees at the bottom of desync1  */
   if (peaks[r-1] != 0 && 
       abs(peaks[r]-peaks[r-1]) < 2) {
      if (PRINT_DEBUG) printf(" 5 ");
      return false;
   }
   /* detect annoying blocks of two, e.g. lines 147-9 in desync3.pgm */
   if (r < peaks.size()-1) {
      const int sum = peaks[r] + peaks[r-1] + peaks[r-2];
      if (abs(sum) <= 3) {
         if (PRINT_DEBUG) printf(" 6 ");
         return false;
      }
   }
   /* detect blocks of three shifted lines */
   if (r < peaks.size()-2) {
      const int sum = peaks[r] + peaks[r-1] + peaks[r-2] + peaks[r-3];
      if (abs(sum) <= 4) {
         if (PRINT_DEBUG) printf(" 7 ");
         return false;
      }
   }
   /* any other noticable difference between two peak coordinates */
   if (abs(peaks[r] - peaks[r-1]) > 5) {
      if (PRINT_DEBUG) printf(" 8 ");
      return true;
   }

   /* if all else fails, shift it just in case */
   if (PRINT_DEBUG) printf(" X ");
   return true;
}

/* function to do the bulk of the work */
bool Image::synchronise() {
   std::vector<int> peaks(height);
   std::vector<size_t> rows_to_be_shifted;

   /* find shift distances for each row */
   for (size_t r = 1; r < height; r++) {
      Row xcorr = cross_correlate(rows[r], rows[r-1]);
      peaks[r] = peak(xcorr);
   }
   /* determine which rows need to be shifted */
   for (size_t r = 1; r < height; r++) {
      bool shift_it =    row_should_be_shifted(r, peaks) 
                      && !is_in_array(r-1, rows_to_be_shifted);
      if (shift_it) rows_to_be_shifted.push_back(r);
      if (PRINT_DEBUG) {
         printf("row=%3zu peak at x=%4d prev=%s chosen=%s\n", 
                r, peaks[r], (rows[r].has_been_shifted?"yes":"no "), 
                (shift_it?"yes":"no "));
      }
   }
   /* print out some info about what's been shifted */
   if (PRINT_DEBUG) {
      printf("Rows shifted = ");
      for (auto x : rows_to_be_shifted) 
         printf("%zu ", x);
      printf("\n");
   } else {
      printf("%3zu rows shifted\n", rows_to_be_shifted.size());
   }

   /* If there's nothing to do, we may as well back out */
   if (rows_to_be_shifted.size() == 0) 
      return false;

   /* apply shifts to rows that actually need it */
   for (size_t r = 1; r < height; r++) {     
      if (is_in_array(r, rows_to_be_shifted)) {
         if (is_in_array(r-1, rows_to_be_shifted)) {
            /* update the shift distance based on any previous shifts, so we 
               don't throw it too far */
            peaks[r] = peak(cross_correlate(rows[r], rows[r-1]));
         }
         if (PRINT_DEBUG) {
            printf("shifting row %3zu ", r);
         }
         rows[r].shift(peaks[r]); /* actually apply the shift */

         if (PRINT_DEBUG) {
            printf(" by %3d, new peak = %3d\n", 
                   peaks[r], peak(cross_correlate(rows[r], rows[r-1])));
         }
      } /* if is_in_array */
   } /* loop over rows */

   return true;
}

/* calculate the x-coordinate of the max value of the cross correlation */
int Image::peak(const Row& r) const {
   double max_height = -1e200;
   int peak_position;
   std::vector<double> x(r.width);
   for (size_t i = 0; i < x.size(); i++) {
      /* fudge factor at the end is to account for some problems I ran into */
      x[i] = i - 0.5*r.width + (r.width%2==1?0.5:0); 
   }
   for (size_t i = 0; i < r.width; i++) {
      /* magnitude(i) is the magnitude of the complex number at the ith column */
      if (r.magnitude(i) > max_height) {
         max_height = r.pixels[i][0];
         peak_position = ROUND(x[i]);
      }
   }
   return peak_position;
}

Row Image::cross_correlate(const Row& row1, 
                           const Row& row2) {
   /* get the largest stretch of pixels thats covered by both row1 and row2 */
   const size_t first_index  = MAX(row1.starting_index, row2.starting_index);
   const size_t subrow_width = row1.overlapping_pixels_with(row2);
   const Row r1 = row1.subrow(first_index, subrow_width);
   const Row r2 = row2.subrow(first_index, subrow_width);

   ASSERT( r1.width == r2.width );

   const Row fft1       = r1.fft();
   const Row fft2       = r2.fft();
   const Row multiplied = fft1 * fft2.conjugate();
   Row inversed         = multiplied.inverse_fft();

   /* shift the phase of the periodic function, so the peak is (ideally) at 
      the middle of the curve */
   inversed.recentre();
   return inversed;
}

/* save the shifted image to file */
void Image::save() const {
   const size_t pos_extension     = filename.find_last_of(".");
   const std::string no_extension = filename.substr(0, pos_extension);
   const std::string output_name  = no_extension + "_synced.pgm";
   std::ofstream file(output_name);
   ASSERT( file.is_open() );

   /* file info at the top */
   file << "P5\n";
   file << "# Title:\n";
   const size_t pos_filename       = filename.find_last_of("/");
   const std::string base_filename = filename.substr(pos_filename+1);
   file << "# " << base_filename << " after resynchronisation\n"; 
   file << width << " " << height << " 255\n";

   for (size_t r = 0; r < height; r++) {
      const size_t start_r = rows[r].starting_index;
      const size_t width_r = rows[r].width;
      size_t j = 0;
      for (size_t c = 0; c < width; c++) {
         /* BLANK_PIXEL is set in the Image() constructor */
         double temp;
         if      (c < start_r)         temp = BLANK_PIXEL;
         else if (c < start_r+width_r) temp = rows[r].magnitude(j++);
         else                          temp = BLANK_PIXEL;
         file << (unsigned char)temp;
      }
   }
   file.close();
   printf("Saved to %s\n", output_name.c_str());
}
