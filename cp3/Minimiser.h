#ifndef MINIMISER_H
#define MINIMISER_H

#include <vector>
#include <stdlib.h>
#include "DataPoints.h"

class Minimiser {
public:
   Minimiser(const DataPoints& data);

   Params get_final_parameters() const            { return m_params_curr; }
   ModelFunction model()                          { return model_function; }
   std::string model_name() const                 { return m_model_func_name; }

   void set_max_iterations(size_t max)            { m_iterations_max = max; m_iterations_curr = 0; }
   void set_epsilon(double e)                     { m_epsilon = e; }
   void set_initial_grid_search_volumes(size_t n) { m_N_grid_volumes = n; } 

   void set_param_limits(const Params&, const Params&);
   void set_function_to_minimise_implementation(FunctionToMinimise f, const std::string& function_name);
   void set_model_function_implementation(ModelFunction f, const std::string& function_name);

   void minimise();   // does most of the legwork in here

   void n_dimensional_grid_search(Params params, 
                                  const Params& pmax,
                                  const Params& pmin,
                                  const size_t N_volumes,
                                  const size_t level, 
                       /*output*/ double* min_grid_value,
                       /*output*/ Params* params_at_min);
   void n_dimensional_minimisation(Params params, 
                                   const int level, 
                                   const Params& d_params,
                        /*output*/ double* min_value, 
                        /*output*/ Params* params_at_min);
   void plot(const std::vector<DataPoints>& points);
   void find_parameter_errors(const Params& fitted_params);

private:
   FunctionToMinimise function_to_minimise;  // pointer to the function we're minimising (chi squared)
   std::string m_function_name;              // eg "chi squared"
   ModelFunction model_function;             // function ptr to output y as a function of x (linear/cubic/whatever)
   std::string m_model_func_name;            // eg "linear"
   std::string m_model_description;          // eg "y = a + b*x"
   DataPoints m_datapoints;                  // x, y values of dataset to minimise params for
   size_t m_iterations_curr;                 // current count of completed iterations
   size_t m_iterations_max;                  // max times to iterate on the params
   double m_epsilon;                         // error acceptability limit
   Params m_params_curr;                     // current (iterated) values of parameters
   Params m_params_max;                      // given limits of parameters to work within
   size_t m_N_grid_volumes;                  // number of volumes to check along each dof during initial grid search
   Params m_params_min;                      // parameters at minimum value
};

#endif