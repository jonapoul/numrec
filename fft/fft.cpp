#include "../global.h"

int main(int argc, char** argv) {
   /* expecting a single digit as an argument, from 0-4 */
   ASSERT( argc > 1 );
   ASSERT( isdigit(argv[1][0]) );
   const int digit = argv[1][0] - '0';
   ASSERT( digit > 0 && digit <= 4 );

   char filename[100];
   snprintf(filename, 100, "fft/images/desync%d.pgm", digit);
   Image img(filename);

   const CImg<double> start = img.to_cimg();

   bool result = true;
   for (size_t run_count = 0; result == true; run_count++) {
      if (run_count == 2) break;
      result = img.synchronise();
   }
   //img.fill_in_blanks(); // find best-guess colours for the missing pixels

   const CImg<double> end = img.to_cimg();
   //synchronised.save("fft/images/desync1_sync.pgm");
   (start, end).display("original vs synchronised", false);
   return 0;
}



//DO MORE CHECKS IN Image::ROWSHOULDBESHIFTED
//ALSO MAKE SURE THE UPDATED PEAK ISNT WORSE THAN THE TWO NEW ONES
//AND ESPECIALLY CHECK UP ON ROWS 288-289
// if both sides are worse, check a range of pixel shifts either side to find the best match
   // or if both are zero
   // or if bother are nonzero