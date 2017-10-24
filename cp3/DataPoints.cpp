#include "../global.h"
#include "DataPoints.h"

DataPoints::DataPoints(const std::string& filename, const std::string& dataset_name) {
   std::ifstream file(filename);
   if (!file) {
      printf("no file in %s\n", __FUNCTION__);
      exit(1);
   }
   std::string temp_str;
   while (std::getline(file, temp_str)) {
      std::stringstream ss(temp_str);
      double temp;
      ss >> temp;
      this->x.push_back(temp);
      ss >> temp;
      this->y.push_back(temp);
      ss >> temp;
      this->e.push_back(temp);
   }
   this->name = dataset_name;
}

DataPoints::DataPoints(const DataPoints& dp) 
   : x(dp.x), y(dp.y), e(dp.e), name(dp.name) { }

DataPoints::DataPoints(const XArray& X, 
                       const Params& params, 
                       const std::string& dataset_name,
                       double(*model_function)(const double,const Params&) ) {
   this->x = X;
   this->y.resize(x.size(), 0.0);
   this->e.resize(x.size(), 0.0);
   this->name = dataset_name;
   for (size_t i = 0; i < x.size(); i++) {
      y[i] = model_function(x[i], params);
   }
}