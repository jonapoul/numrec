#ifndef FFTHANDLER_H
#define FFTHANDLER_H

#include <string>
#include "../global.h"

class Row;
class FFTPlotter;

class FFTHandler {
public:
   std::string filename; // image file in question
   size_t      height;   // number of rows
   size_t      width;    // number of columns
   double*     pixels;   // entire image in one array
   Row*        rows;     // array of pixel rows

public:
   FFTHandler();
   ~FFTHandler();
   
   void initialise(const char* fn);
   void synchronise();
   void read_pgm_file(const char* fn);
   void extract_rows();

   bool row_should_be_shifted(const size_t r, const std::vector<int>& peaks);
   int peak(const Row& r) const;
   inline double magnitude(fftw_complex a) const;
   CImg<double> to_cimg();
   Row xcorr(const Row& row1, const Row& row2);
};

#endif