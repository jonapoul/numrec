#ifndef COORDS_H
#define COORDS_H

#include <string>
#include <iostream>

class Coords {
public:
   Coords();
   Coords(const double lat, const double lon);
   Coords(const std::string& lat, const std::string& lon);
   Coords(const Coords& c);

   std::string str() const;
   void print() const;
   friend std::ostream& operator<<(std::ostream& os, const Coords& c);

   double latitude;
   double longitude;
};

#endif