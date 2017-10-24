#include <vector>
#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

#include "../global.h"
#include "Minimiser.h"
#include "DataPoints.h"

Minimiser::Minimiser(const DataPoints&  data) 
   : m_datapoints(data) { }

void Minimiser::set_param_limits(const Params& params_min, const Params& params_max) {
   exit_if_error(params_min.size() != params_max.size(), __FILE__, __FUNCTION__, __LINE__);
   m_params_max = params_max;
   m_params_min = params_min;
}

void Minimiser::set_function_to_minimise(const std::string& function_name,
                                         double(*f)(const DataPoints&,
                                                    const Params&,
                                                    double(*model_function)(const double,
                                                                            const Params&))) { 
   function_to_minimise = f; 
   m_function_name = function_name;
}

void Minimiser::minimise() {
   const size_t N_params = m_params_min.size();
   Params grid_search_params(N_params, 0.0);
   Params min_parameters(N_params, 0.0);
   double min_grid_value = 1e200;

   const int N_volumes = 50;
   n_dimensional_grid_search(grid_search_params, m_params_max, m_params_min, N_volumes, N_params, 
                             &min_grid_value, &min_parameters);

   // set the initial starting parameters based on this search
   m_params_init = m_params_curr = min_parameters;

   /*
      NOW DO THE ACTUAL MINIMISATION YOU IDIOT
   */
}

/* basically a for loop with an arbitrary number of nested loops, depending on params.size() */
void Minimiser::n_dimensional_grid_search(Params params, 
                                          const Params& pmax,
                                          const Params& pmin,
                                          const size_t N_volumes,
                                          size_t level, 
                               /*output*/ double* min_grid_value,
                               /*output*/ Params* params_at_min) {
   if (level == 0) {
      // if we're at the bottom of the nested loops, calculate the value, compare to min_value, and back off
      Params temp_params;
      double this_value = function_to_minimise(m_datapoints, params, model_function);
      if (this_value < *min_grid_value) { 
         *min_grid_value = this_value;
         *params_at_min = params;
      }
      return;
   } else {
      // otherwise, loop through all values of this parameter, then iterate down to the next parameter
      size_t param_index = params.size() - level;
      const double d_param = (pmax[param_index]-pmin[param_index])/(double)N_volumes;
      for (size_t i = 0; i < N_volumes; i++) {
         params[param_index] = pmin[param_index] + d_param*(i+0.5);
         n_dimensional_grid_search(params, pmax, pmin, N_volumes, level-1, 
                                   min_grid_value, params_at_min);
      }
   }
}

void Minimiser::plot(const std::vector<DataPoints>& data_arr, const std::string& title) {
   plt::clf();
   const std::string colours = "rgmybc";
   for (size_t i = 0; i < data_arr.size(); i++) {
      // plot actual points
      const auto e = data_arr[i].e;
      const auto x = data_arr[i].x;
      const auto y = data_arr[i].y;
      std::string format = (i % 2 == 0) ? " o" : " -";
      format[0] = colours[i % colours.length()];
      plt::named_plot(data_arr[i].name, x, y, format);
      
      // y error lines for each point
      for (size_t j = 0; j < e.size(); j++) {
         plt::plot({x[j], x[j]}, {y[j]+e[j], y[j]-e[j]}, "k-");
      }
   }
   plt::legend();

   std::stringstream ss;
   ss << title << "\n" << m_function_name;
   ss << " minimum of " << function_to_minimise(m_datapoints,m_params_curr,model_function);
   ss << "\n" << m_iterations_curr << " iterations";
   plt::title(ss.str());
   plt::show();
}