#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <string>

typedef std::vector<double> array;

struct CoordsArray {
   array x;
   array y;
   std::string name;
   CoordsArray() { }
   CoordsArray(int N) {
      x    = array(N, 0.0);
      y    = array(N, 0.0);
      name = "";
   }
};


#endif