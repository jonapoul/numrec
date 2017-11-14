#ifndef FFTPLOTTER_H
#define FFTPLOTTER_H

class FFTPlotter {
public:
   static void plot(const Row& r1,
                    const Row& r2,
                    const std::string& titlestr="");
   static void plot(const Row& r);
};

#endif