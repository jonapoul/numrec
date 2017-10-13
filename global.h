#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <string>

#define MIN(x,y) (x<y)?x:y
#define MAX(x,y) (x>y)?x:y

typedef std::vector<double> array;

enum { EULER, RK2, RK4, ANALYTIC, METHOD_COUNT };
enum { dEdx_eq_RHO, dVdx_eq_NegE };

struct CoordsArray {
   array x;
   array y;
   std::string name;
   CoordsArray() { }
   CoordsArray(int N) {
      x    = array(N, 0.0);
      y    = array(N, 0.0);
      name = "";
   }
};

bool is_in_range(double min, double x, double max, double epsilon=1e-6);

#endif