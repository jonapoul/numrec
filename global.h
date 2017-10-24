#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <string>

#define MIN(x,y) (x<y)?x:y
#define MAX(x,y) (x>y)?x:y
#define GET_VARIABLE_NAME(var) (#var)
#define __POSITION__ __FILE__,__FUNCTION__,__LINE__ // used when calling exit_if_false()


// just to make it more obvious what I'm using them for (from CP3)
typedef std::vector<double> Params;
typedef std::vector<double> YArray;
typedef std::vector<double> XArray;
typedef std::vector<double> EArray;

enum { EULER, RK2, RK4, ANALYTIC, METHOD_COUNT };
enum { dEdx_eq_RHO, dVdx_eq_NegE };

bool is_in_range(double min, double x, double max, double epsilon=1e-6);
void exit_if_false(bool condition, const char* file, const char* func, int line);

#endif