#ifndef FEATURE_H
#define FEATURE_H

#include <string>
#include "Date.h"

enum { N, NNE, NE, ENE, 
       E, ESE, SE, SSE,
       S, SSW, SW, WSE, 
       W, WNW, NW, NNW };

class Feature {
public:
   Feature();
   Feature(const Feature& f);
   Feature(const std::string line);

private:
   int id_;           // station ID number
   std::string name_; // station name
   double x0_;        //
   double latitude_;  // coords
   double longitude_; // coords
   Date date_;        // observation date
   double x3_;        //
   double x4_;        //
   double x5_;        //
   double x6_;        //
   int direction_;    // wind direction (16 point compass)
   double x7_;        //
   double x8_;        //
   char tendency_;    // Pressure Tendency (Pa/s)
   double x10_;       //
   double x11_;       //
   int type_;         // Weather type index
};

#endif