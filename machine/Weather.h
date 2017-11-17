#ifndef WEATHER_H
#define WEATHER_H

#include <string>
#include "Date.h"
#include "Feature.h"

struct Coords { double lat; double lon; };

enum { FULL, ADVANCED, BASIC }; // datastream type

class Weather {
public:
   Weather(const std::string& file);
   ~Weather();

   std::string filename()    const { return filename_; }
   Date        start_date()  const { return observation_start_; }
   int         datastream()  const { return datastream_; }
   int         num_entries() const { return data_length_; }
   Feature*    data()        const { return data_; }

private:
   std::string filename_;
   Date observation_start_;
   int datastream_;
   Feature* data_;
   size_t data_length_;
   std::vector<std::string> target_names_;
   std::vector<std::string> feature_names_;

   bool load();
   void set_target_names();
   void set_station_data();
   void set_feature_names();

   double get_relative_time(const Feature& f) const;
   static double get_distance(const Coords& c1,
                              const Coords& c2);
   static Coords get_new_coords(const Coords& c,
                                const double distance,
                                const double direction);
};

#endif