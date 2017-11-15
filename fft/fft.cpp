#include "../global.h"

int main(int argc, char** argv) {
   /* grab the inputfile number, the iteration limit, and output debug switch
      from command line arguments */
   std::string filename;
   size_t iteration_limit;
   bool print_debug;
   get_arguments(argc, argv, &filename, &iteration_limit, &print_debug);

   Image img(filename, print_debug);
   printf("Filename     = %s\n", img.filename.c_str());
   printf("Height       = %zu\n", img.height);
   printf("Width        = %zu\n", img.width);
   printf("Debug output = %s\n\n", (print_debug ? "true" : "false"));

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