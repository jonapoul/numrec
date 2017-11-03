#include "../global.h"
#include "DataPoints.h"

DataPoints::DataPoints() {
   //blank
}

DataPoints::DataPoints(const std::string& filename, const std::string& dataset_name) {
   std::ifstream file(filename);
   EXIT_IF_FALSE( file.good() );
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
                       ModelFunction model_function) {
   x = X;
   y.resize(x.size(), 0.0);
   e.resize(x.size(), 0.0);

   name = dataset_name;
   for (size_t i = 0; i < x.size(); i++) {
      y[i] = model_function(x[i], params);
   }
}

DataPoints::DataPoints(const size_t N_points) {
   x.resize(N_points, 0);
   y.resize(N_points, 0);
   e.resize(N_points, 0);
   name = "";
}