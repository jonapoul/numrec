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

   DataPoints(const std::string& filename, 
              const std::string& dataset_name);
   DataPoints(const DataPoints& dp);
   DataPoints(const XArray& x, 
              const Params& params, 
              const std::string& dataset_name, 
              double(*model_function)(const double,const Params&));
};

#endif