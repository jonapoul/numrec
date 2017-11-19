#include <string>
#include <iostream>
#include "Station.h"
#include "Coords.h"

using std::string;

Station::Station() 
         : id(""), 
           name(""), 
           coords(0., 0.) { }

Station::Station(const Station& s)
         : id(s.id),
           name(s.name),
           coords(s.coords),
           distance(s.distance) { }

Station::Station(const string& ID,
                 const string& n,
                 const Coords& c)
         : id(ID),
           name(n),
           coords(c) { }

std::string Station::str() const {
   char buf[200];
   snprintf(buf, 200, "[%5s, %29s, %8.4f, %8.4f, %7.3f]", 
            id.c_str(), name.c_str(), coords.latitude, coords.longitude, distance);
   return string(buf);
}

void Station::print() const {
   std::cout << *this << '\n';
}

std::ostream& operator<<(std::ostream& os, const Station& s) {
   return os << s.str();
}