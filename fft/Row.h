#ifndef ROW_H
#define ROW_H

#include <fftw3.h>
class Image;

class Row {
public:
   Image*        parent;
   fftw_complex* pixels;
   size_t        width;
   size_t        shifted_distance;
   size_t        row_index;
   size_t        starting_index;
   bool          has_been_shifted;

public:
   Row();
   Row(const Row& r);
   ~Row();

   void print_debug() const;
   void recentre();
   void initialise(Image* im, const size_t index);
   void shift(const size_t distance, const int direction);
   Row conjugate() const;
   Row fft() const;
   Row inverse_fft() const;
   Row subrow(const size_t first, const size_t length) const;
   double magnitude(const size_t index) const;
   size_t overlapping_pixels_with(const Row& r) const;
   
   static fftw_complex* allocate(const size_t size);
   static void deallocate(const Row& r);
   static void deallocate(fftw_complex* a);
   
   Row operator*(const Row&) const;
   Row& operator=(const Row& r);
};

#endif