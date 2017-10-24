#ifndef DATAPOINTS_H
#define DATAPOINTS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "../global.h"

class DataPoints {
public:
   XArray x;
   YArray y;
   EArray e;
   std::string name;

   // read data from filename and store it in this object
   DataPoints(const std::string& filename, 
              const std::string& dataset_name);
   // clone another object
   DataPoints(const DataPoints& dp);
   // generate points based on an x range, some parameters, and the function to combine these into y(x)
   DataPoints(const XArray& x, 
              const Params& params, 
              const std::string& dataset_name, 
              double(*function)(const double,const Params&));
};

#endif