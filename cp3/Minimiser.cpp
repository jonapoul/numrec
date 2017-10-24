#include <vector>
#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

#include "../global.h"
#include "Minimiser.h"
#include "DataPoints.h"

Minimiser::Minimiser(const DataPoints&  data) 
      : m_datapoints(data) {
   function_to_minimise = nullptr;
   model_function = nullptr;
}

void Minimiser::set_param_limits(const Params& params_min, const Params& params_max) {
   exit_if_false(params_min.size() == params_max.size(),    __POSITION__);

   m_params_max = params_max;
   m_params_min = params_min;
}

/* function f takes as arguments:
      - DataPoints object = datapoints read in from the file, with associated uncertainties
      - Params object     = array of parameters to pass to model_function()
      - model_function    = function to which we're fitting the parameters (linear in this case)
   f = chi-squared for this, but I thought I'd try to generalise it a bit */
void Minimiser::set_function_to_minimise(const std::string& function_name,
                                         double(*f)(const DataPoints&,
                                                    const Params&,
                                                    double(*model_function)(const double,
                                                                            const Params&))) { 
   function_to_minimise = f; 
   m_function_name = function_name;
}

void Minimiser::minimise() {
   // ensure that min/max params objects have same nonzero length
   exit_if_false( m_params_min.size()  == m_params_max.size(), __POSITION__);
   exit_if_false( m_params_min.size()  != 0,                   __POSITION__);
   exit_if_false( function_to_minimise != nullptr,             __POSITION__);
   exit_if_false( model_function       != nullptr,             __POSITION__);
   
   const size_t N_params = m_params_min.size();
   Params grid_search_params(N_params, 0.0); // parameters to be passed and changed through recursion
   Params min_parameters(N_params, 0.0); // values of each parameter that minimises the function
   double min_value = 1e200;
   const int N_volumes = 50;

   // recursively find a good spot to start by trying parm values within params_min->params_max
   n_dimensional_grid_search(grid_search_params, m_params_max, m_params_min, N_volumes, N_params, 
                             &min_value, &min_parameters);

   m_params_curr = min_parameters; // set the initial starting parameters based on this search
   Params d_params(N_params, 0.0);
   for (size_t i = 0; i < N_params; i++) {
      // search range for each param set to half of that from n_dimensional_grid_search()
      d_params[i] = (m_params_max[i] - m_params_min[i]) / (2.0 * N_volumes);
   }

   double difference = 1e200;
   double current_minimum = 1e200;

   while (m_iterations_curr < m_iterations_max && difference > m_epsilon) {
      double previous_minimum = current_minimum;
      current_minimum = 1e200;

      //uses m_params_curr as the starting point for recursion, updates the pointer's value as it finds new minima.
      n_dimensional_minimisation(m_params_curr, N_params, d_params, &current_minimum, &m_params_curr);

      // update variables for rechecking, then loop around for next iteration
      difference = abs(previous_minimum - current_minimum);
      m_iterations_curr++;
      if (current_minimum > previous_minimum) { // if this value is higher that last, half increments
         for (auto& dp : d_params) {
            dp /= 2.0;
         }
      }
   }
}

/* Basically a recursive for-loop with an arbitrary number of nested loops, depending on params.size().
   Used to find a good guess for initial parameters to use within the ranges pmax to pmin */
void Minimiser::n_dimensional_grid_search(Params params,          // test parameters
                                          const Params& pmax,     // limits of each parameter
                                          const Params& pmin,
                                          const size_t N_volumes, // number of grid volumes along each param axis
                                          const size_t level,     // how deep into the recursion we are
                               /*output*/ double* min_value,      // smallest function value from entire search
                               /*output*/ Params* params_at_min) {// param values at that point
   if (level == 0) {
      // if we're at the bottom of the nested loops, calculate the value, compare to min_value, and back off
      double this_value = function_to_minimise(m_datapoints, params, model_function);
      if (this_value < *min_value) { 
         *min_value = this_value;
         *params_at_min = params;
      }
      return;
   } else {
      // otherwise, loop through all values of this parameter, then iterate down to the next parameter
      size_t param_index = params.size() - level;
      const double d_param = (pmax[param_index]-pmin[param_index])/(double)N_volumes;
      for (size_t i = 0; i < N_volumes; i++) {
         // update the test parameter value to the midpoint of that grid volume
         params[param_index] = pmin[param_index] + d_param*(i+0.5);
         // decrement level to update the next parameter in the next iteration
         n_dimensional_grid_search(params, pmax, pmin, N_volumes, level-1, 
                                   min_value, params_at_min);
      }
   }
}

/* Recursive method of evaluating one iteration of the minimisation routine for an arbitrary number of parameters.
   Two output values passed back as pointers to the minimum function value and the parameters at that point */
void Minimiser::n_dimensional_minimisation(Params params, 
                                           const int level,        // how deep into the recursion we are
                                           const Params& d_params, // step size along each parameter axis
                                /*output*/ double* min_value,      // smallest function value from entire search
                                /*output*/ Params* params_at_min) {// param values at that point
   if (level == 0) {
      // if we're at the bottom of the nested loops, calculate the value, compare to min_value, and back off
      //Params p_plus_dp = params, p_minus_dp = params;
      double this_value = function_to_minimise(m_datapoints, params, model_function);
      if (this_value < *min_value) { 
         *min_value = this_value;
         *params_at_min = params;
      }
      return;
   } else {
      // indicates which parameter we're dealing with in this recursion
      size_t param_index = params.size() - level;
      double dp = d_params[param_index];
      double p = params[param_index];

      // go down a level with param p -> p+dp
      params[param_index] = p + dp;
      n_dimensional_minimisation(params, level-1, d_params, min_value, params_at_min);

      // same with p -> p-dp
      params[param_index] = p - dp;
      n_dimensional_minimisation(params, level-1, d_params, min_value, params_at_min);
   }
}

void Minimiser::plot(const std::vector<DataPoints>& data_arr) {
   plt::clf();
   const std::string colours = "rgmybc";
   for (size_t i = 0; i < data_arr.size(); i++) {
      // plot actual points
      const auto e = data_arr[i].e;
      const auto x = data_arr[i].x;
      const auto y = data_arr[i].y;
      std::string format = (i == 0) ? " o" : " -";
      format[0] = colours[i % colours.length()];
      plt::named_plot(data_arr[i].name, x, y, format);
      
      // y error lines for each point
      for (size_t j = 0; j < e.size(); j++) {
         plt::plot({x[j], x[j]}, {y[j]+e[j], y[j]-e[j]}, "k-");
      }
   }
   plt::legend();

   std::stringstream ss;
   ss << m_function_name << " min=" << function_to_minimise(m_datapoints,m_params_curr,model_function);
   ss << "; iterations=" << m_iterations_curr;
   plt::title(ss.str());
   plt::show();
}

void Minimiser::reset() {
   m_function_name = "";
   function_to_minimise = nullptr;
   model_function = nullptr;
   m_datapoints.x = {};
   m_datapoints.y = {};
   m_datapoints.e = {};
   m_iterations_curr = 0;
   m_iterations_max = 0;
   m_epsilon = 0;
   m_params_min = {};
   m_params_max = {};
   m_params_curr = {};
}