#ifndef FEATURE_H
#define FEATURE_H

#include <string>
#include "Date.h"

class Feature {
public:
   Feature();
   Feature(const Feature& f);
   Feature(const std::string line);

   void print() const;

   std::string name()     const { return name_; }
   std::string dir()      const { return direction_; }
   std::string tendency() const { return tendency_; }
   int id()    const { return id_; }
   int type()  const { return type_; }
   Date date() const { return date_; }
   double elevation()  const { return elevation_; }
   double latitude()   const { return latitude_; }
   double longitude()  const { return longitude_; }
   double time()       const { return time_; }
   double gust()       const { return gust_; }
   double temp()       const { return temp_; }
   double visibility() const { return visibility_; }
   double speed()      const { return speed_; }
   double pressure()   const { return pressure_; }
   double dewpoint()   const { return dewpoint_; }

private:
   static int counter;
   int index_;             // unique ID number for each Feature object
   int id_;                // station ID number
   std::string name_;      // station name
   double elevation_;      // metres
   double latitude_;       // degrees
   double longitude_;      // degrees
   Date date_;             // date
   double time_;           // mins since midnight
   double gust_;           // mph
   double temp_;           // deg C
   double visibility_;     // m
   std::string direction_; // wind direction (16 point compass)
   double speed_;          // mph
   double pressure_;       // hPa
   std::string tendency_;  // Pressure Tendency (Pa/s)
   double dewpoint_;       // Dew Point (deg C)
   double humidity_;       // %
   int type_;              // Weather type index
};

#endif