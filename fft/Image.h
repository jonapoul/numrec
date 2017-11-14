#ifndef Image_H
#define Image_H

#include <string>
#include "../global.h"

class Row;

class Image {
public:
   std::string filename; // image file in question
   size_t      height;   // number of rows
   size_t      width;    // number of columns
   double*     pixels;   // 1D array of all pixels
   Row*        rows;     // array of pixel rows

   int BLANK_PIXEL;

public:
   Image(const std::string& file);
   ~Image();
   
   bool synchronise();
   void save() const;
   Row cross_correlate(const Row& row1, const Row& row2);
   int peak(const Row& r) const;
   bool row_should_be_shifted(const size_t r, const std::vector<int>& peaks);
};

#endif