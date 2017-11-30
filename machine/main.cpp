#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "../global.h"
#include "Weather.h"

int main(int argc, char** argv) {
   const std::string filename = "machine/data/basic.txt";
   const int lines_to_pick = 10;
   Weather w(filename, lines_to_pick);
   w.fix();
   w.delete_feature("Station Name");
   w.delete_feature("Date");

   const arma::mat data = w.matrix();
   //data.raw_print("features:");
   //for (auto f : w.feature_names()) std::cout << '\t' << f << '\n';

}
