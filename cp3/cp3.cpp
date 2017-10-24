#include <iostream>
#include <vector>
#include <math.h>
#include <assert.h>

#include "../global.h"
#include "Minimiser.h"
#include "DataPoints.h"

double chi_sq(const DataPoints& data,
              const Params& params,
              double (*model_function)(const double,const Params&)) {
   double chisq = 0.0;
   for (size_t i = 0; i < data.x.size(); i++) {
      double y_model =  model_function(data.x[i], params);
      double chi = (data.y[i] - y_model) / data.e[i];
      chisq += pow(chi, 2.0);
   }
   return chisq;
}

double linear(const double x, const Params& params) {
   exit_if_false( params.size() >= 2, __POSITION__ );
   const double m = params[0];
   const double c = params[1];
   return m*x + c;
}

double quadratic(const double x, const Params& params) {
   exit_if_false( params.size() >= 3, __POSITION__);
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   return a + b*x + c*x*x;
}

double cubic(const double x, const Params& params) {
   exit_if_false( params.size() >= 4, __POSITION__);
   const double a = params[0];
   const double b = params[1];
   const double c = params[2];
   const double d = params[3];
   return a + b*x + c*x*x + d*x*x*x;
}

int main() {
   std::string filename = "cp3/testData.txt";
   DataPoints file_data(filename, "Data");              // read in data from file and store them in DataPoints object

   Minimiser m(file_data);                              // initialise a Minimiser engine
   m.set_function_to_minimise("Chi-squared", &chi_sq);  // function that we're trying to minimise by changing the parameters
   m.set_model_function(&linear);                       // function that we're fitting the data to
   m.set_epsilon(1e-7);                                 // minimum acceptable error in minimise()
   m.set_max_iterations(10000);                          // limit the iteration count

   double lim = 0.5;
   Params upper_linear = { lim,  lim};
   Params lower_linear = {-lim, -lim};
   m.set_param_limits(upper_linear, lower_linear);      // limits of minimisation search for each parameter

   m.minimise();                                        // actually do the work
   Params linear_params = m.get_final_parameters();      // retrieve the resulting parameters that minimise the function
   DataPoints linear_data(file_data.x, linear_params, "Linear", &linear);


   m.plot({file_data, linear_data}); // plot the line against datapoints

   return 0;
}