#ifndef PLOTTER_H
#define PLOTTER_H

class RootFinder;
class ChargeDistribution;

class Plotter {
public:
   void plot(const RootFinder& rf, bool show_plot = true);
   void plot(const ChargeDistribution& cd, const std::string& title, bool show_plot = true);
};

#endif