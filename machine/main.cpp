#include <iostream>

#include "../global.h"
#include "Weather.h"

int main(int argc, char** argv) {
   const int file_slice = 0;
   const std::string filename = "data/basic.txt";
   Weather w(filename, file_slice);

   printf("###################################\n");
   printf("     Step 3: Inspect the data:\n");
   printf("###################################\n");


}