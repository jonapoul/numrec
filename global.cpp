#include <string>
#include "global.h"

bool is_in_range(double min, double x, double max, double epsilon) { 
   return (min-epsilon < x && x < max+epsilon); 
}