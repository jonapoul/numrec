#include "../global.h"

int main(int argc, char** argv) {
   /* grab the inputfile number and the iteration limit from command line */
   std::string filename;
   size_t iteration_limit;
   get_arguments(argc, argv, &filename, &iteration_limit);

   const bool print_debug = false;
   Image img(filename, print_debug);
   printf("Filename = %s\n", img.filename.c_str());
   printf("Height   = %zu\n", img.height);
   printf("Width    = %zu\n\n", img.width);

   /* do the legwork, backing out if we've exceeded the run limit or if no 
      lines were shifted */
   for (size_t count = 1; count <= iteration_limit; count++) {
      printf("Iteration %2zu/%zu: ", count, iteration_limit);

      /* synchronise() only returns false if no rows are chosen to be shifted */
      if ( img.synchronise() == false ) 
         break;
   }

   /* save the synced image to fft/images/desync[X]_synced.pgm */
   img.save();
   return 0;
}