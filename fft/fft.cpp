#include "../global.h"
#include <CImg/CImg.h>
using namespace cimg_library;

/* 
   expecting to be run as:

         bin/fft <image number> <run_limit>
         
   where 1 <= image_number <= 4 and run_limit >= 1
*/
int main(int argc, char** argv) {
   std::string filename;
   size_t run_limit;
   get_arguments(argc, argv, &filename, &run_limit);

   Image img(filename); /* holds all the image data in Row objects */

   CImgList<double> comparison(run_limit+1);
   comparison(0) = img.to_cimg();

   for (size_t run_count = 0; run_count < run_limit; run_count++) {
      printf("Run %zu/%zu...\n", run_count+1, run_limit);
      img.synchronise();
      comparison(run_count+1) = img.to_cimg();
   }
   //img.fill_in_blanks(); // find best-guess colours for the missing pixels

   comparison.display("original vs each iteration", false);
   return 0;
}