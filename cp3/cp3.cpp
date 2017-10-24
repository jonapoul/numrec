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
   exit_if_error( params.size()<2, __FILE__, __FUNCTION__, __LINE__ );
   const double m = params[0];
   const double c = params[1];
   return m*x + c;
}

int main() {
   std::string filename = "cp3/testData.txt";
   DataPoints file_data(filename, "data");              // read in data from file and store them in DataPoints object

   Minimiser m(file_data);                              // initialise a Minimiser engine
   m.set_function_to_minimise("Chi-squared", &chi_sq);  // function that we're trying to minimise by changing the parameters
   m.set_model_function(&linear);                       // function that we're fitting the data to
   m.set_epsilon(1e-5);                                 // minimum acceptable error in minimise()
   m.set_max_iterations(100);                           // limit the iteration count

   Params upper = { 5,  5};
   Params lower = {-5, -5};
   m.set_param_limits(upper, lower);                    // limits of minimisation search for each parameter

   m.minimise();                                        // actually do the work
   Params final_params = m.get_final_parameters();      // retrieve the results

   DataPoints modelled_data(file_data.x, final_params, "modelled", &linear);         // calculate line points
   m.plot({file_data, modelled_data}, "Input data compared to modelled parameters"); // plot the line against datapoints

   return 0;
}