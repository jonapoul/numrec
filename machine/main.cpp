#include <iostream>
#include <vector>
#include <string>

#include "../global.h"
#include "Weather.h"

void print_vector(const vector<string>& vec) {
   for (size_t i = 0; i < vec.size(); i++) 
      printf("%zu/%zu %s\n", i, vec.size(), vec[i].c_str());
   printf("\n");
}

int main(int argc, char** argv) {
   const std::string filename = "machine/data/basic.txt";
   Weather w(filename);

   printf("###################################\n");
   printf("     Step 3: Inspect the data:\n");
   printf("###################################\n");

   auto vec = w.feature_data("Temperature");
   
}