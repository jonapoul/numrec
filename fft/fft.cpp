#include "../global.h"

int main(int argc, char** argv) {
   /* grab the inputfile number and the iteration limit from command line */
   std::string filename;
   size_t run_limit;
   get_arguments(argc, argv, &filename, &run_limit);

   Image img(filename);

   /* do the legwork, backing out if we've exceeded the run limit or if no 
      lines were shifted */
   for (size_t run_count = 0; run_count < run_limit; run_count++) {
      printf("Run %2zu/%zu: ", run_count+1, run_limit);
      if ( !img.synchronise() ) 
         break;
   }

   /* save the synced image to fft/images/desyncX_synced.pgm */
   img.save();
   return 0;
}