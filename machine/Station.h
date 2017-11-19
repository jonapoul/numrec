#ifndef STATION_H
#define STATION_H

#include <string>
#include <iostream>
#include "Coords.h"

class Station {
public:
   Station();
   Station(const Station& s);
   Station(const std::string& ID,
           const std::string& n,
           const Coords& c);

   std::string str() const;
   void print() const;
   friend std::ostream& operator<<(std::ostream& os, const Station& d);

   std::string id; 
   std::string name; 
   Coords coords;
   double distance = -1;
};

#endif