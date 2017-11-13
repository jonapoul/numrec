#include "../global.h"
#include "Row.h"

Row::Row() 
      : parent(nullptr), 
        pixels(nullptr), 
        width(0), 
        shifted_distance(0), 
        row_index(0), 
        starting_index(0), 
        has_been_shifted(false) { }

Row::Row(const Row& r) 
      : parent(r.parent), 
        width(r.width), 
        shifted_distance(r.shifted_distance), 
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

void Row::initialise(Image* im,
                     const size_t index) {
   /* this is separate from the constructors due to pointer-related shenanigans */
   parent           = im;
   width            = parent->width;
   shifted_distance = 0;
   row_index        = index;
   starting_index   = 0;

   pixels = allocate(width);
   for (size_t i = 0; i < width; i++) {
      /* grab the appropriate pixel values from the Image parent */
      pixels[i][0] = parent->pixels[width*row_index + i];
      pixels[i][1] = 0.0;
   }
}

/* values of "direction" defined in global.h */
void Row::shift(const size_t distance, 
                const int direction) {
   ASSERT( direction == RIGHT || direction == LEFT );
   if (distance == 0) 
      return;

   this->width = parent->width - distance;
   this->shifted_distance = distance;
   this->starting_index = (direction == LEFT) ? 0 : distance;
   this->has_been_shifted = true;

   // copy over relevant pixels to temporary array
   fftw_complex* temp = allocate(width);
   if (direction == LEFT) {
      for (size_t i = 0; i < width; i++) {
         temp[i][0] = pixels[i + distance][0];
         temp[i][1] = pixels[i + distance][1];
      }
   } else {
      for (size_t i = 0; i < width; i++) {
         temp[i][0] = pixels[i][0];
         temp[i][1] = pixels[i][1];
      }
   }

   // resize pixels array and bring them back over
   deallocate(pixels);
   pixels = allocate(width);
   for (size_t i = 0; i < width; i++) {
      pixels[i][0] = temp[i][0];
      pixels[i][1] = temp[i][1];
   }
   deallocate(temp);
}

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
   for (size_t i = 0; i < result.width; i++) {
      result.pixels[i][0] /= (double)result.width;
      result.pixels[i][1] /= (double)result.width;
   }
   return result;
}

Row Row::conjugate() const {
   Row result(*this);
   for (size_t i = 0; i < width; i++) {
      result.pixels[i][1] *= -1;
   }
   return result;
}

void Row::print_debug() const {
   printf("Parent           = %s\n", (parent ? parent->filename.c_str() : "nullptr"));
   printf("Width            = %zu\n", width);
   printf("Shifted Distance = %zu\n", shifted_distance);
   printf("Row Index        = %zu\n", row_index);
   printf("Starting Index   = %zu\n", starting_index);

   size_t j = 0;
   for (size_t i = 0; i < parent->width; i++) {
      if      (i < starting_index)       printf("%3zu=%3.0f ", i, parent->BLANK_PIXEL);
      else if (i < starting_index+width) printf("%3zu=%3.0f ", i, pixels[j++][0]);
      else                               printf("%3zu=%3.0f ", i, parent->BLANK_PIXEL);
      if ( (i+1) % 20 == 0 ) printf("\n");
   }
   printf("\n");
}

/* creates a shortened row object, starting from index 'first' of the pixels array and
   stretching for 'length' pixels. Basically for allowing cross-correlations betweened shifted 
   and non-shifted rows, only taking into account the pixel indices covered by both rows */
Row Row::subrow(const size_t first, 
                const size_t length) const {
   if (length == 0) 
      return *this;
   ASSERT( first < this->width );
   Row sub;
   sub.parent           = this->parent;
   sub.width            = length;
   sub.shifted_distance = parent->width - length;
   sub.row_index        = this->row_index;
   sub.starting_index   = first;
   sub.pixels           = allocate(length);
   for (size_t i = 0; i < length; i++) {
      sub.pixels[i][0] = this->pixels[first + i][0];
      sub.pixels[i][1] = this->pixels[first + i][1];
   }
   return sub;
}

double Row::magnitude(const size_t index) const {
   return sqrt(pixels[index][0]*pixels[index][0] + pixels[index][1]*pixels[index][1]);
}

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

size_t Row::overlapping_pixels_with(const Row& r) const {
   size_t count = 0;
   for (size_t i = 0; i < parent->width; i++) {
      if (i > this->starting_index && i < this->starting_index + this->width &&
          i > r.starting_index     && i < r.starting_index + r.width) {
         count++;
      }
   }
   return count;
}

Row Row::operator*(const Row& r) const {
   Row result(*this);
   for (size_t i = 0; i < width; i++) {
      // (A + iB)(C + iD) = (AC-BD) + i(BC+AD)
      const double A = this->pixels[i][0];
      const double B = this->pixels[i][1];
      const double C =     r.pixels[i][0];
      const double D =     r.pixels[i][1];
      result.pixels[i][0] = A*C - B*D;
      result.pixels[i][1] = B*C + A*D;
   }
   return result;
}

Row& Row::operator=(const Row& r) {
   this->parent           = r.parent;
   this->width            = r.width;
   this->shifted_distance = r.shifted_distance;
   this->row_index        = r.row_index;
   this->starting_index   = r.starting_index;
   this->has_been_shifted = r.has_been_shifted;
   this->pixels           = allocate(r.width);
   for (size_t i = 0; i < r.width; i++) {
      this->pixels[i][0] = r.pixels[i][0];
      this->pixels[i][1] = r.pixels[i][1];
   }
   return *this;
}