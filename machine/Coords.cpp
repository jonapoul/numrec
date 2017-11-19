#include <iostream>
#include <string>

#include "Coords.h"
using std::string;

Coords::Coords() 
         : latitude(0.),
           longitude(0.) { }

Coords::Coords(const double lat, 
               const double lon)
         : latitude(lat),
           longitude(lon) { }

Coords::Coords(const string& lat, 
               const string& lon)
         : latitude(stod(lat)),
           longitude(stod(lon)) { }

Coords::Coords(const Coords& c)
         : latitude(c.latitude),
           longitude(c.longitude) { }

string Coords::str() const {
   char buf[50]; 
   snprintf(buf, 50, "[%f, %f]", latitude, longitude); 
   return string(buf);
}

void Coords::print() const {
   std::cout << *this << '\n';
}

std::ostream& operator<<(std::ostream& os, const Coords& c) {
   return os << c.str();
}