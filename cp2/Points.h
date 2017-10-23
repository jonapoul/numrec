#ifndef POINTS_H
#define POINTS_H

#include "../global.h"

class Points {
public:
   std::vector<double> x;
   std::vector<double> y;
   std::string name;

   Points() { }

   Points(const std::vector<double>& X, const std::vector<double>& Y, const std::string str) 
      : x(X), y(Y), name(str) { }

   size_t size() { return x.size(); }

   Points operator-(const Points& that) const {
      std::vector<double> y_diff(y.size());
      for (int i = 0; i < (int)y.size(); i++) 
         y_diff[i] = this->y[i] - that.y[i];
      return Points(this->x, y_diff, "");
   }
};

#endif