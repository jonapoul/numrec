#include <math.h>
#include "../global.h"
#include "Row.h"

Row::Row() 
         : parent(nullptr), 
           pixels(nullptr), 
           width(0), 
           row_index(0), 
           starting_index(0), 
           has_been_shifted(false) { }

Row::Row(const Row& r) 
         : parent(r.parent), 
           width(r.width), 
           row_index(r.row_index), 
           starting_index(r.starting_index),
           has_been_shifted(r.has_been_shifted) {
   this->pixels = allocate(width);
   for (size_t i = 0; i < width; i++) {
      this->pixels[i][0] = r.pixels[i][0];
      this->pixels[i][1] = r.pixels[i][1];
   }
}

Row::~Row() {
   deallocate(pixels);
}

/* this is used instead of a constructor because I can't call a constructor on 
   top of a heap allocation call in the Image() constructor */
void Row::initialise(Image* im,
                     const size_t index) {
   this->parent           = im;
   this->width            = parent->width;
   this->row_index        = index;
   this->starting_index   = 0;
   this->pixels = allocate(width);
   for (size_t i = 0; i < width; i++) {
      /* grab the appropriate pixel values from the Image parent */
      this->pixels[i][0] = parent->pixels[width*row_index + i];
      this->pixels[i][1] = 0.0;
   }
}

/* shorten the entire row, clipping off one end of pixels depending on the 
   direction. See the header in Row.h for more description */
void Row::shift(const int distance) {
   /* if we don't need to shift, just back out now */
   if (distance == 0) return;

   /* a negative peak distance indicates a right shift, and vice versa */
   enum { LEFT, RIGHT };
   const int direction = (distance < 0) ? RIGHT : LEFT;

   this->width            = parent->width - abs(distance);
   this->starting_index   = (direction == LEFT) ? 0 : abs(distance);
   this->has_been_shifted = true;

   /* copy over relevant pixels to temporary array */   
   fftw_complex* temp = allocate(width);
   const int offset = (direction == LEFT) ? distance : 0;
   for (size_t i = 0; i < width; i++) {
      temp[i][0] = this->pixels[i + offset][0];
      temp[i][1] = this->pixels[i + offset][1];
   }

   /* resize pixels array and copy them back over */
   deallocate(pixels);
   pixels = allocate(width);
   for (size_t i = 0; i < width; i++) {
      this->pixels[i][0] = temp[i][0];
      this->pixels[i][1] = temp[i][1];
   }
   deallocate(temp);
}

/* before applying this function, the peak of the inversed FFT is at the edges
   of the range, so shift the phase by pi so it's a bit easier to work with */
void Row::recentre() {
   fftw_complex* temp = allocate(width);
   for (size_t i = 0; i < width/2; i++) {
      temp[i][0] = pixels[i + width/2][0];
      temp[i][1] = pixels[i + width/2][1];
   }
   for (size_t i = width/2; i < width; i++) {
      temp[i][0] = pixels[i - width/2][0];
      temp[i][1] = pixels[i - width/2][1];
   }

   for (size_t i = 0; i < width; i++) {
      pixels[i][0] = temp[i][0];
      pixels[i][1] = temp[i][1];
   }
   deallocate(temp);
}

/* I realise that creating and destroying plans all the time isn't the most 
   efficient way of doing things, but efficiency wasn't what I was aiming for */
Row Row::fft() const {
   Row result(*this);
   fftw_plan p = fftw_plan_dft_1d(width, this->pixels, result.pixels, FFTW_FORWARD, FFTW_ESTIMATE);
   fftw_execute(p);
   fftw_destroy_plan(p);
   return result;
}

Row Row::inverse_fft() const {
   Row result(*this);
   fftw_plan p = fftw_plan_dft_1d(width, this->pixels, result.pixels, FFTW_BACKWARD, FFTW_ESTIMATE);
   fftw_execute(p);
   fftw_destroy_plan(p);
   return result;
}

Row Row::conjugate() const {
   Row result(*this);
   for (size_t i = 0; i < width; i++) {
      result.pixels[i][1] *= -1;
   }
   return result;
}

/* creates a shortened row object, starting from index 'first' of the pixels 
   array and stretching for 'length' pixels. Basically for allowing 
   cross-correlations between two rows of different widths, only taking into 
   account the pixel indices covered by both rows */
Row Row::subrow(const size_t first, 
                const size_t length) const {
   if (length == 0) return *this;
   ASSERT( first <  this->width );
   ASSERT( first >= this->starting_index );

   Row sub;
   sub.parent           = this->parent;
   sub.width            = length;
   sub.row_index        = this->row_index;
   sub.starting_index   = first;
   sub.pixels           = allocate(length);
   for (size_t i = 0; i < length; i++) {
      sub.pixels[i][0] = this->pixels[first - this->starting_index + i][0];
      sub.pixels[i][1] = this->pixels[first - this->starting_index + i][1];
   }
   return sub;
}

double Row::magnitude(const size_t index) const {
   return sqrt(pixels[index][0]*pixels[index][0] + 
               pixels[index][1]*pixels[index][1]);
}

/* so we don't have loads of ugly pointer casts littered all over the place */
fftw_complex* Row::allocate(const size_t size) {
   fftw_complex* output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
   return output;
}

void Row::deallocate(const Row& r) {
   deallocate(r.pixels);
}

void Row::deallocate(fftw_complex* a) {
   fftw_free(a);
}

/* counts how many pixels are defined by "this" and row r. This will be less 
   than this->width if either of the rows has been previously shifted */
size_t Row::overlapping_pixels_with(const Row& r) const {
   size_t count = 0;
   for (size_t i = 0; i < parent->width; i++) {
      if (   i >= this->starting_index 
          && i <  this->starting_index + this->width 
          && i >= r.starting_index     
          && i <  r.starting_index + r.width) {
         count++;
      }
   }
   return count;
}

/* multiplies every element of two rows, basically a dot product with complex 
   numbers */
Row Row::operator*(const Row& r) const {
   Row result(*this);
   for (size_t i = 0; i < width; i++) {
      /* (A + iB)(C + iD) = (AC-BD) + i(BC+AD) */
      const double A = this->pixels[i][0];
      const double B = this->pixels[i][1];
      const double C =     r.pixels[i][0];
      const double D =     r.pixels[i][1];
      result.pixels[i][0] = A*C - B*D;
      result.pixels[i][1] = B*C + A*D;
   }
   return result;
}