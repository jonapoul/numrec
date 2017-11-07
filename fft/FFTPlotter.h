#ifndef FFTPLOTTER_H
#define FFTPLOTTER_H

class FFTPlotter {
public:
   void plot(const std::vector<double>& x, 
             const std::vector<fftw_complex*>& y,
             const size_t row,
             const size_t row_prev);

   void plot(const std::vector<double>& x, 
             const std::vector<size_t>& estimated_lines, 
             const std::vector<double>& diffs);

private:
   std::string colours = "rgbmcyk";
};

#endif