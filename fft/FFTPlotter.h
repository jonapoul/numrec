#ifndef FFTPLOTTER_H
#define FFTPLOTTER_H

class FFTPlotter {
public:
   static void plot(const std::vector<double>& x, 
                    fftw_complex* y,
                    const size_t row,
                    const size_t row_prev);
   static void plot(const Row& r1,
                    const Row& r2,
                    const std::string& titlestr="");
   static void plot(const Row& r);
};

#endif