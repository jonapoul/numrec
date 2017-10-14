#ifndef PLOTTER_H
#define PLOTTER_H

#include <string>
#include "../global.h"

class RootFinder;
class PNJunction;

enum { SAVE_FIG, SHOW_FIG };

class Plotter {
public:
   Plotter(int flag, char f_char);
   void plot(const PNJunction& pn);

private:
   std::string colours = "bgrmyc";
   std::string format;
   std::string fig_dir = "./cp2/fig";
   int output_flag;

   void plot_rho(const PNJunction& pn);
   void plot_E  (const PNJunction& pn);
   void plot_V  (const PNJunction& pn);

   void plot_E_differences(const PNJunction& pn);
   void plot_V_differences(const PNJunction& pn);

   void plot_euler(const PNJunction& pn);
   void plot_rk2  (const PNJunction& pn);
   void plot_rk4  (const PNJunction& pn);

   inline std::string filepath(const char* filename) const;
};

#endif