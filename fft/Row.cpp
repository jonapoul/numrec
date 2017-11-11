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
   fftw_free(pixels);
}

void Row::initialise(FFTHandler* h,
                     const size_t index) {
   /* this is separate from the constructors due to pointer-related shenanigans */
   parent = h;
   width = parent->width;
   shifted_distance = 0;
   row_index = index;
   starting_index = 0;

   pixels = allocate(width);
   for (size_t i = 0; i < width; i++) {
      /* grab the appropriate pixel values from the FFTHandler parent */
      pixels[i][0] = parent->pixels[width*row_index + i];
      pixels[i][1] = 0.0;
   }
}

/* values of "direction" defined in global.h */
void Row::shift(const size_t distance, 
                const int direction) {
   has_been_shifted = true;
   width = parent->width - distance;
   fftw_complex* temp = allocate(width);
   starting_index = (direction == LEFT) ? distance : 0;
   for (size_t i = 0; i < width; i++) {
      temp[i][0] = pixels[starting_index + i][0];
      temp[i][1] = pixels[starting_index + i][1];
   }
   fftw_free(pixels);
   pixels = allocate(width);
   for (size_t i = 0; i < width; i++) {
      pixels[i][0] = temp[i][0];
      pixels[i][1] = temp[i][1];
   }
   fftw_free(temp);
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
   fftw_free(temp);
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
   char ptrbuf[200];
   snprintf(ptrbuf, 200, "valid = %s", parent->filename.c_str());
   printf("Parent           = %s\n", (parent ? ptrbuf : "nullptr"));
   printf("Width            = %zu\n", width);
   printf("Shifted Distance = %zu\n", shifted_distance);
   printf("Row Index        = %zu\n", row_index);
   printf("Starting Index   = %zu\n", starting_index);

   size_t j = 0;
   for (size_t i = 0; i < parent->width; i++) {
      if      (i < starting_index)       printf("p[%3zu]=%3d ", i, -1);
      else if (i < starting_index+width) printf("p[%3zu]=%3d ", i, (int)pixels[j++][0]);
      else                               printf("p[%3zu]=%3d ", i, -1);
      if ( (i+1) % 14 == 0 ) printf("\n");
   }
   printf("\n");
}

Row Row::subrow(const size_t first, 
                const size_t length) const {
   Row result;
   result.parent           = this->parent;
   result.width            = length;
   result.shifted_distance = this->width - length;
   result.row_index        = this->row_index;
   result.starting_index   = first;
   result.pixels = allocate(length);
   for (size_t i = 0; i < length; i++) {
      result.pixels[i][0] = this->pixels[first + i][0];
      result.pixels[i][1] = this->pixels[first + i][1];
   }
   return result;
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
   this->pixels           = allocate(width);
   for (size_t i = 0; i < this->width; i++) {
      this->pixels[i][0] = r.pixels[i][0];
      this->pixels[i][1] = r.pixels[i][1];
   }
   this->has_been_shifted = r.has_been_shifted;
   return *this;
}