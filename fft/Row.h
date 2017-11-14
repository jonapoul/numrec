#ifndef ROW_H
#define ROW_H

#include <fftw3.h>
class Image;

/*
   If we originally have a row like this:
   _________________________________________
   | A | B | C | D | E | F | G | H | I | J |
   ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻
   then starting_index = 0, width = 10, has_been_shifted = false

   But if we now shift the row to the right by 3:
   _________________________________________
   | X | X | X | A | B | C | D | E | F | G |
   ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻
   where X indicates a pixel that's not tracked by this object any more.
   - starting_index = 3 because the first pixel that's stored corresponds 
     to column 3 of the image row. The first 3 pixels are filled in with pure 
     black in Image::save() when we're done. 
   - width = 7, because we've removed the pixels H,I,J from the right end of
     the row.
   - has_been_shifted is true because we've applied a shift and we shouldn't 
     need to do it again.
   _________________________________________
   | D | E | F | G | H | I | J | X | X | X |
   ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻
   Now we've got a row thats been shifted by 3 to the left, starting_index = 0 
   and width = 7 again. The three X's indicate pixels that are filled in by 
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