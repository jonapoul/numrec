#include <vector>
#include <sstream>
#include <matplotlib-cpp/matplotlibcpp.h>
namespace plt = matplotlibcpp; 

#include "../global.h"
#include "Minimiser.h"
#include "DataPoints.h"

Minimiser::Minimiser(const DataPoints& data) 
      : function_to_minimise(nullptr), model_function(nullptr), m_datapoints(data) { }

void Minimiser::set_param_limits(const Params& params_min, 
                                 const Params& params_max) {
   EXIT_IF_FALSE( params_min.size() == params_max.size() );
   m_params_max = params_max;
   m_params_min = params_min;
}

/* function f takes as arguments:
      - DataPoints object = datapoints read in from the file, with associated uncertainties
      - Params object     = array of parameters to pass to model_function()
      - model_function    = function to which we're fitting the parameters (linear in this case)
   f = chi-squared for this, but I thought I'd try to generalise it a bit */
void Minimiser::set_function_to_minimise_implementation(FunctionToMinimise f, 
                                                        const std::string& function_name) { 
   function_to_minimise = f; 
   m_function_name = function_name;
}

void Minimiser::set_model_function_implementation(ModelFunction f, 
                                                  const std::string& function_name) {
   model_function = f;
   m_model_func_name = function_name;
   if (function_name == "linear")      m_model_description = "y = a + b*x";
   if (function_name == "quadratic")   m_model_description = "y = a + b*x + c*x^2";
   if (function_name == "cubic")       m_model_description = "y = a + b*x + c*x^2 + d*x^3";
   if (function_name == "polynomial")  m_model_description = "y = a + b*x + c*x^2 + d*x^3 + e*x^4 ...";
   if (function_name == "sinusoidal")  m_model_description = "y = a + b*sin(c*x+d)";
   if (function_name == "power")       m_model_description = "y = a + b*x^(c)";
   if (function_name == "exponential") m_model_description = "y = a + b*c^x";
   if (function_name == "logarithmic") m_model_description = "y = a + b*log(c*x + d)";
   if (function_name == "gaussian")    m_model_description = "y = a + b*exp( [(x-c)/d]^2 / 2 )";
   if (function_name == "step")        m_model_description = "y = { a if x < c } or { b for x > c }";
   EXIT_IF_FALSE(m_model_description[0] == 'y'); // if none of these conditions were met
}

void Minimiser::minimise() {
   // ensure that various conditions have been met before continuing
   EXIT_IF_FALSE( m_params_min.size()  == m_params_max.size() ); // grid max/minima have the same dimensions
   EXIT_IF_FALSE( m_params_min.size()  != 0);                    // we have at least one parameter
   EXIT_IF_FALSE( function_to_minimise != nullptr);              // main function (chisq) has been assigned
   EXIT_IF_FALSE( model_function       != nullptr);              // model function (linear) has been assigned

   const size_t N_params = m_params_min.size();
   printf("Function       = %s\n", m_model_func_name.c_str());
   printf("N_volumes      = %zu\n", m_N_grid_volumes);
   printf("N_params       = %zu\n", N_params);
   printf("Max iterations = %zu\n", m_iterations_max);
   printf("\t%s\n\n", m_model_description.c_str());
   
   Params grid_search_params(N_params, 0.0); // parameters to be passed and changed through recursion
   Params min_parameters(N_params, 0.0); // values of each parameter that minimises the function
   double min_value = 1e200;

   // recursively find a good spot to start by trying parm values within params_min->params_max
   printf("Starting initial grid search...\n");
   n_dimensional_grid_search(grid_search_params, m_params_max, m_params_min, m_N_grid_volumes, N_params, 
                             &min_value, &min_parameters);
   printf("\nFinished initial grid search!\nInitial grid-searched parameters:\n");
   for (size_t i = 0; i < min_parameters.size(); i++) {
      double p = min_parameters[i];
      printf("\t%c = %s%f\n", (char)('a'+i), (p<0?"":" "), p);
   }
   printf("\n");

   m_params_curr = min_parameters; // set the initial starting parameters based on this search
   Params d_params(N_params, 0.0);
   for (size_t i = 0; i < N_params; i++) {
      // search range for each param set to half of that from n_dimensional_grid_search()
      d_params[i] = (m_params_max[i] - m_params_min[i]) / (2.0 * m_N_grid_volumes);
   }

   double difference    = 1e200;
   double current_chisq = 1e200;

   printf("Starting iteration...\n");
   while (m_iterations_curr < m_iterations_max && difference > m_epsilon) {
      double previous_chisq = current_chisq;
      current_chisq = 1e200;

      // uses m_params_curr as the starting point for recursion, updates the pointer's values as it finds new minima
      n_dimensional_minimisation(m_params_curr, N_params, d_params, &current_chisq, &m_params_curr);

      // update variables for rechecking, then loop around for next iteration
      difference = abs(previous_chisq - current_chisq);
      m_iterations_curr++;
      // if this chisq value is higher than the last, halve increments in all directions
      if (current_chisq > previous_chisq) {
         for (auto& dp : d_params) {
            dp /= 2.0;
         }
      }
      // print current progress percentage
      printf("%6.2f%% \r", 100*(double)m_iterations_curr/(double)m_iterations_max);
   }
   printf("\nFinished iteration!\n");
   printf("Iteration count = %zu\n", m_iterations_curr);
   printf("Epsilon         = %.0e\n", m_epsilon);
   printf("Minimum %s = %f\n", m_function_name.c_str(), current_chisq);
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
      // print out current progress percentage
      static size_t index = 0;
      printf("%6.2f%% \r", index*100.0/pow(N_volumes, params.size()) );
      index++;

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
   ss << m_function_name << "=" << function_to_minimise(m_datapoints,m_params_curr,model_function);
   ss << " for " << data_arr[1].name << " fit\n";
   ss << "iterations=" << m_iterations_curr;
   ss << "; epsilon=" << m_epsilon;
   plt::title(ss.str());
   plt::xlabel("x");
   plt::ylabel("y");
   //plt::show();
   plt::save("cp3/fig/fitted_line.png");

}

void Minimiser::find_parameter_errors(const Params& fitted_params) {
   const size_t N_params = fitted_params.size();
   const size_t N_points = 1e4;
   const std::string colours = "rcmybg";
   for (size_t i = 0; i < N_params; i++) {
      Params temp_params = fitted_params;
      const double p     = fitted_params[i];

      // spatial resolution in our search along the param axis
      const double dp = p / (double)N_points;
      // minimum value of chisq for this parameter
      const double chisq_min = function_to_minimise(m_datapoints,fitted_params,model_function);
      
      double chisq = 0;
      double max_p;
      double smallest_distance = 1e200;
      double p_at_chisq_plus_1, p_at_chisq_minus_1;

      // calculate chisq along the parameter axis and test whether it's more than chisq_min+8
      for (size_t j = 0; chisq < chisq_min+8; j++) {
         max_p = p + j*dp;
         temp_params[i] = max_p;
         chisq = function_to_minimise(m_datapoints, temp_params, model_function);

         double distance_to_1 = abs(chisq-chisq_min-1);
         // if this value of the parameter is close to delta(chisq)=1, save the values
         if (distance_to_1 < smallest_distance) {
            smallest_distance = distance_to_1;
            p_at_chisq_plus_1 = max_p;
         }
      }
      // reset values to originals, then do the same but the other side of the minimum value
      temp_params = fitted_params;
      chisq = 0;
      smallest_distance = 1e200;
      double min_p;
      for (size_t j = 0; chisq < chisq_min+8; j++) {
         min_p = p - j*dp;
         temp_params[i] = min_p;
         chisq = function_to_minimise(m_datapoints, temp_params, model_function);

         double distance_to_1 = abs(chisq-chisq_min-1);
         if (distance_to_1 < smallest_distance) {
            smallest_distance = distance_to_1;
            p_at_chisq_minus_1 = min_p;
         }
      }

      // we now have our x limits, so generate points between then
      double max = MAX(max_p, min_p), min = MIN(max_p, min_p);
      DataPoints data_points(N_points);
      data_points.x = generate_smooth_x_values({min,max}, N_points);
      for (size_t j = 0; j < N_points; j++) {
         temp_params[i] = data_points.x[j];
         data_points.y[j] = function_to_minimise(m_datapoints, temp_params, model_function);
      }
      data_points.name = (char)('a'+i);

      // now plot them
      plt::clf();
      char buf[200];
      snprintf(buf, sizeof(buf)-1, " = %s%f +- %f", (fitted_params[i]<0?"":" "), 
                                                     fitted_params[i], 
                                                     abs(fitted_params[i]-p_at_chisq_plus_1));
      std::string title = data_points.name + buf;
      printf("\t%s\n", title.c_str());
      plt::title(title + "\nwhere " + m_model_description);
      std::string format = " -";
      format[0] = colours[i % colours.length()];

      plt::plot(data_points.x, data_points.y,         format); // chi-sq parabola
      plt::plot({min_p,max_p}, {chisq_min,chisq_min}, "k-");   // horizontal line indicating minimum of parabola

      plt::plot(XArray(2,p_at_chisq_minus_1),{chisq_min,chisq_min+1},"k-"); // vertical line on the left
      plt::plot(XArray(2,p_at_chisq_plus_1), {chisq_min,chisq_min+1},"k-"); // vertical line on the right

      plt::ylabel(m_function_name);
      plt::xlabel(data_points.name);
      // plt::show();

      std::stringstream ss;
      ss << "cp3/fig/" << data_points.name << "_parabola.png";
      plt::save(ss.str());
   }
   printf("Plots saved in cp3/fig/\n");
}