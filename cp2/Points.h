#ifndef POINTS_H
#define POINTS_H

#include "../global.h"

class Points {
public:
   array x;
   array y;
   std::string name;

   Points() { }
   Points(const array& X, const array& Y, std::string str) {
      x = X; y = Y; name = str;
   }
   Points operator-(Points that) {
      array y_diff(y.size());
      for (int i = 0; i < (int)y.size(); i++) 
         y_diff[i] = this->y[i] - that.y[i];
      return Points(this->x, y_diff, "");
   }
   Points& operator/=(const double c) {
      for (int i = 0; i < (int)y.size(); i++) 
         this->y[i] /= c;
      return *this;
   }
};

#endif