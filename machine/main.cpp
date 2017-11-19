#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "../global.h"
#include "Weather.h"

int main(int argc, char** argv) {
   std::string filename;
   int lines_to_pick;
   get_arguments(argc, argv, &filename, &lines_to_pick);

   Weather w(filename, lines_to_pick);
   //w.test();
}