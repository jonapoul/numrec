#ifndef WEATHER_H
#define WEATHER_H

#include <string>
#include "Date.h"

enum { FULL, ADVANCED, BASIC }; // datastream type

class Weather {
public:
   Weather(const std::string& file,
           const int lines = 0);
   int number_of_entries() const;

private:
   std::string filename;
   int lines_to_read;
   Date observation_start;
   int datastream;

   bool load();
   void set_target_names();
   void set_station_data();
   void set_feature_names();
};

#endif