#ifndef ROW_H
#define ROW_H

#include <fftw3.h>
class Image;

/*
   If we originally have a row like this (where A-J indicate colour values):
   _________________________________________
   | A | B | C | D | E | F | G | H | I | J |
   ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻
   where starting_index = 0, width = 10, has_been_shifted = false

   But if we now shift the row to the right by 3:
   _________________________________________
   |   |   |   | A | B | C | D | E | F | G |
   ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻
   where a blank box indicates a pixel that's not tracked by this object any 
   more. These are filled in with pure black in Image::save() when we're done. 
   - starting_index = 3 because the first pixel that's stored corresponds 
     to column 3 of a filled-out row.
   - width = 7, because we've removed the pixels H,I,J from the right end of
     the row.
   - has_been_shifted is true because we've applied a shift and we shouldn't 
     need to do it again.
   _________________________________________
   | D | E | F | G | H | I | J |   |   |   |
   ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻
   Now we've got a row thats been shifted by 3 to the left, starting_index = 0 
   and width = 7. The three blanks indicate pixels that are filled in by 
   Image::save() again. A,B,C are discarded.

   Hopefully this makes sense to you, it seems to work at least :)
*/
class Row {
public:
   Image*        parent;
   fftw_complex* pixels;
   size_t        width;
   size_t        row_index;
   size_t        starting_index;
   bool          has_been_shifted;

public:
   Row();
   Row(const Row& r);
   ~Row();

   void initialise(Image* im, const size_t index);
   void recentre();
   void shift(const int distance);
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
};

#endif