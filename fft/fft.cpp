#include "../global.h"

int main() {

   FFTHandler handler;
   handler.initialise("fft/images/desync1.pgm");
   CImg<double> original = handler.to_cimg();

   handler.synchronise(); // actually do the work
   handler.synchronise();
   //handler.fill_in_blanks(); // find best-guess colours for the missing pixels

   CImg<double> synchronised = handler.to_cimg();
   synchronised.save("fft/images/desync1_sync.pgm");
   //(synchronised).display("original vs synchronised", false);
   return 0;
}
