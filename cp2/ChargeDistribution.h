#include <vector>
#include <math.h>
#include <string>
using std::vector;

class ChargeDistribution {
private:
   double x0;
   double x1;
   double x2;
   double x3;
   double x4;
   double k;

   double shape(double x0, double x1, double x2);


public:
   ChargeDistribution();
   double evaluate(double x);
   void get_XY_values(double start, double end, int N_points, vector<double>* x, vector<double>* y);
   void plot(const std::string& title);

};