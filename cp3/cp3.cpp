/*
   9/10
   Dont use so many typedefs, bit overbuilt
*/

// system headers
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

// my headers
#include "../global.h"
#include "Minimiser.h"
#include "DataPoints.h"
#include "ModelFunctions.h"

// MINUIT2
#include "Math/IFunction.h"
#include "Math/Functor.h"
#include "Minuit2/Minuit2Minimizer.h"

// global array of points from the data file, only used to access x/y values in chisq_2() for minuit
DataPoints global_datapoints;

// for Minimiser
double chisq_1(const DataPoints& data, const Params& params, ModelFunction func) {
   double chisq = 0.0;
   for (size_t i = 0; i < data.x.size(); i++) {
      double y_model = func(data.x[i], params);
      double chi = (data.y[i] - y_model) / data.e[i];
      chisq += chi * chi;
   }
   return chisq;
}

// for MINUIT2
double chisq_2(const double* params) {
   const double a = params[0];
   const double b = params[1];

   double chisq = 0.0;
   for (size_t i = 0; i < global_datapoints.x.size(); i++) {
      double y_model = a + b*global_datapoints.x[i];
      double chi = (global_datapoints.y[i] - y_model) / global_datapoints.e[i];
      chisq += chi * chi;
   }
   return chisq;
}

int main() {
   const double epsilon = 1e-9;
   const size_t max_iterations = 1e5;

   printf("\n\n----------------------STARTING MY MINIMISATION----------------------\n");
   const std::string filename = "/home/jon/numrec/cp3/datasets/testData.txt";
   const DataPoints file_data(filename, "Data");   // read in data from file

   Minimiser m(file_data);                         // initialise a Minimiser engine
   m.set_function_to_minimise(chisq_1);            // function that we're minimising by fitting the parameters
   m.set_model_function(linear);                   // function that we're fitting the data to
   m.set_epsilon(epsilon);                         // minimum acceptable error in minimise()
   m.set_max_iterations(max_iterations);           // limit the iteration count
   m.set_initial_grid_search_volumes(1e2);         // how many grid volumes to check along each param axis

   const Params upper = { 1.1,  0.1 }; 
   const Params lower = { 0.9, -0.1 };
   m.set_param_limits(upper, lower);               // limits of initial grid search for each parameter

   m.minimise();                                   // actually do the work
   Params final_params = m.get_final_parameters(); // retrieve the resulting parameters that minimise chisq

   const size_t N_points = 1e3;
   const XArray smooth_x = generate_smooth_x_values(file_data.x, N_points);
   const DataPoints fitted_points(smooth_x, final_params, m.model_name(), m.model()); 

   m.plot({file_data, fitted_points});             // plot the line against datapoints
   m.find_parameter_errors(final_params);          // calculate/plot how chisq changes as we vary each param in turn
   printf("-----------------------ENDING MY MINIMISATION-----------------------\n\n\n\n");




   printf("-------------------STARTING MINUIT2 MINIMISATION--------------------\n");
   global_datapoints = file_data; // accessed in chisq_2()
   ROOT::Math::Minimizer* min = new ROOT::Minuit2::Minuit2Minimizer("minimize"); 

   // minimiser settings
   min->SetMaxFunctionCalls(max_iterations); 
   min->SetTolerance(epsilon);
   min->SetPrintLevel(1); // increase this to print out more verbose results

   ROOT::Math::Functor function(&chisq_2, 2); // 2 parameters 
   min->SetFunction(function);

   double variable[] = { 1.0,  0.0 };  // starting point
   double step[]     = { 1e-3, 1e-3 }; // initial step sizes
   min->SetVariable(0, "a", variable[0], step[0]);
   min->SetVariable(1, "b", variable[1], step[1]);

   // do the minimization
   min->Minimize(); 
   
   printf("Minimum chisq = %f\n", min->MinValue());
   delete min;   
   printf("--------------------ENDING MINUIT2 MINIMISATION--------------------\n\n\n");

   return 0;
}