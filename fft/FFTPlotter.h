#ifndef FFTPLOTTER_H
#define FFTPLOTTER_H

class FFTPlotter {
public:
   void plot(const std::vector<double>& x, 
             fftw_complex* y,
             const size_t row,
             const size_t row_prev);
};

#endif