#ifndef Image_H
#define Image_H

#include <string>
#include <CImg/CImg.h>
using namespace cimg_library;
#include "../global.h"

class Row;

class Image {
public:
   std::string filename; // image file in question
   size_t      height;   // number of rows
   size_t      width;    // number of columns
   double*     pixels;   // entire image in one array
   Row*        rows;     // array of pixel rows

   double BLANK_PIXEL;

public:
   Image(const std::string& file);
   ~Image();
   
   void synchronise();
   void read_pgm_file(const std::string& fn);
   void extract_rows();

   bool row_should_be_shifted(const size_t r, const std::vector<int>& peaks);
   int peak(const Row& r) const;
   CImg<double> to_cimg();
   Row cross_correlate(const Row& row1, const Row& row2);
};

#endif