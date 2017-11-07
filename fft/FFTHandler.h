#ifndef FFTHANDLER_H
#define FFTHANDLER_H

#include <string>
#include "../global.h"

class FFTHandler {
public:
   double* read_pgm_file(const char*   filename,
                   /*output*/  size_t* N_rows,
                   /*output*/  size_t* N_cols);
   void show_as_images(const double* orig_pix,
                       const double* trans_pix,
                       const size_t  cols,
                       const size_t  rows);
   fftw_complex* get_row(const double* pixels,
                         const size_t  N_cols,
                         const size_t  row_index);
   fftw_complex* row_fft(      fftw_complex* row_pixels,
                         const size_t        length,
                         const bool          is_forward=true);
   fftw_complex* conjugate(fftw_complex* input,
                           const size_t  N);
   fftw_complex* multiply(fftw_complex* a,
                          fftw_complex* b,
                          const size_t  N);
   double min_value(fftw_complex* a,
                    const size_t  N);
   fftw_complex* xcorr(fftw_complex* a,
                       fftw_complex* b,
                       const size_t  N);
   void copy(fftw_complex*  input,
             const size_t   N,
             fftw_complex** output);
   double sum_diffs_sq(fftw_complex* a,
                       const size_t  N,
            /*output*/ double*       left,
            /*output*/ double*       right);
};

#endif