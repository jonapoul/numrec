#ifndef FFTHANDLER_H
#define FFTHANDLER_H

#include <string>
#include "../global.h"

class FFTHandler {
public:
   std::string filename; // image file in question
   size_t height; // number of rows
   size_t width; // number of columns
   double* pixels; // entire image
   fftw_complex** rows; // array of each row
   std::vector<size_t> estimated_lines; // lines that should be shifted

public:
   FFTHandler(const char* fn);
   ~FFTHandler();

   fftw_complex* allocate(const size_t N);
   void read_pgm_file();
   void extract_rows();
   fftw_complex* fft_row(fftw_complex* row,
                         const int fft_direction);
   void conjugate(fftw_complex* row);
   fftw_complex* multiply(fftw_complex* a,
                          fftw_complex* b);
   void centre_on_0(fftw_complex* a);
   double peak(fftw_complex* a,
               const std::vector<double>& x);
   fftw_complex* xcorr(const size_t row_index);
};

#endif