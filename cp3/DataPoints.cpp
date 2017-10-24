#include "../global.h"
#include "DataPoints.h"

DataPoints::DataPoints(const std::string& filename, const std::string& dataset_name) {
   std::ifstream file(filename);
   exit_if_false(file.good(), __POSITION__);
   std::string temp_str;
   while (std::getline(file, temp_str)) {
      std::stringstream ss(temp_str);
      double temp;
      ss >> temp;
      x.push_back(temp);
      ss >> temp;
      y.push_back(temp);
      ss >> temp;
      e.push_back(temp);
   }
   name = dataset_name;
}

DataPoints::DataPoints(const DataPoints& dp) 
   : x(dp.x), y(dp.y), e(dp.e), name(dp.name) { }

DataPoints::DataPoints(const XArray& X, 
                       const Params& params, 
                       const std::string& dataset_name,
                       double(*function)(const double,const Params&) ) {
   x = X;
   y.resize(x.size(), 0.0);
   e.resize(x.size(), 0.0);

   std::stringstream ss;
   ss.precision(3);
   ss << dataset_name << "; params=[";
   for (size_t i = 0; i < params.size(); i++) {
      char buf[50];
      snprintf(buf, sizeof(buf)-1, "%.3e", params[i]);
      ss << buf;
      if (i != params.size()-1)
         ss << ", ";
   }
   ss << "]";
   name = ss.str();
   for (size_t i = 0; i < x.size(); i++) {
      y[i] = function(x[i], params);
   }
}