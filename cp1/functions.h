double random_in_range(double min_val, double max_val) {
    return min_val + (max_val-min_val) * (double)rand()/RAND_MAX ;
}

double muon_pdf(double x, double tau) {
   return (1.0/tau) * exp(-x/tau);
}

// two arrays of x and y values for the gaussian plot
typedef std::pair<std::vector<double>, std::vector<double>> CoordsArray;
CoordsArray gaussian(double mu, double sigma, unsigned n_runs, double xmin, double xmax) {
   int N_points = 5000;
   std::vector<double> x(N_points), y(N_points);
   for (int i = 0; i < N_points; i++) {
      x[i] = xmin + i*(xmax-xmin)/(double)N_points;

      // n_runs * 0.045 because this gives (more or less) the peak of the histogram
      double exponent = 0.5 * pow((x[i]-mu)/sigma, 2.0);
      y[i] = n_runs * 0.045 * exp( -exponent );
   }
   return std::make_pair(x, y);
}

CoordsArray exp_decay(double tau, unsigned n_runs, double xmin, double xmax) {
   int N_points = 5000;
   std::vector<double> x(N_points), y(N_points);
   for (int i = 0; i < N_points; i++) {
      x[i] = xmin + i*(xmax-xmin)/(double)N_points;
      y[i] = n_runs * 0.21 * muon_pdf(x[i], tau);
   }
   return std::make_pair(x, y);
}