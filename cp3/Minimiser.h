#ifndef MINIMISER_H
#define MINIMISER_H

#include <vector>
#include <stdlib.h>
#include "DataPoints.h"

class Minimiser {
public:
   Minimiser(const DataPoints& data);

   Params get_final_parameters() const       { return m_params_curr; }
   void set_max_iterations(size_t max)       { m_iterations_max = max; m_iterations_curr = 0; }
   void set_epsilon(double e)                { m_epsilon = e; }
   void set_model_function(double (*f)(const double,const Params&)) { model_function = f; }
   void set_function_to_minimise(const std::string& function_name,
                                 double(*f)(const DataPoints&,const Params&,double(*model_function)(const double,
                                                                                                    const Params&)));
   void set_param_limits(const Params&, const Params&);
   void minimise();
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
   void reset();

private:
   double (*function_to_minimise)(const DataPoints&,   // pointer to the function we're minimising for (chi squared)
                                  const Params&,
                                  double(*model_function)(const double,
                                                          const Params&));
   std::string m_function_name;
   double (*model_function)(const double,
                            const Params&);          // function that we're calculating points for (linear/cubic/whatever)
   DataPoints m_datapoints;                          // x, y values of dataset to minimise params for
   size_t m_iterations_curr;                         // current count of completed iterations
   size_t m_iterations_max;                          // max times to iterate on the params
   double m_epsilon;                                 // error cceptability limit
   Params m_params_curr;                             // current (iterated) values of parameters
   Params m_params_max;                              // given limits of parameters to work within
   Params m_params_min;
};

#endif