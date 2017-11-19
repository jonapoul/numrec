#ifndef WEATHER_H
#define WEATHER_H

#include <string>
#include <math.h>
#include <vector>

#include "Date.h"
#include "Station.h"
#include "WeatherRecord.h"
#include "Coords.h"

enum { FULL, ADVANCED, BASIC }; // datastream type

class Weather {
public:
   Weather(const std::string& file,
           const int lines);

   Date        start_date()   const { return observation_start_; }
   int         datastream()   const { return datastream_; }
   size_t      num_entries()  const { return data_.size(); }
   size_t      num_targets()  const { return target_names_.size(); }
   size_t      num_features() const { return feature_names_.size(); }
   size_t      num_stations() const { return station_data_.size(); }
   std::string filename()     const { return filename_; }
   std::vector<std::string> target_names()  const { return target_names_; }
   std::vector<std::string> feature_names() const { return feature_names_; }

   std::vector<std::string> feature_data(const std::string& feature_name) const;
   std::vector<Station>     station_data(const std::string& station) const;

   void modify(const std::string& feature,
               const std::vector<std::string>& values);
   void append(const std::string& name,
               const std::vector<std::string>& values);
   void select(std::vector<std::string> selected_features);
   void discard();
   void delete_feature(const std::string& feature);
   void export_to_file(const std::string& fname); /* probably not needed */
   std::vector<WeatherRecord> get_observations(const std::string& id="" ,
                                               const std::string& date="",
                                               const std::string& time="",
                                               const std::vector<std::string>& features={});
   std::vector<Station> find_stations(const Coords& origin,
                                      const double offset_dist,
                                      const double offset_theta,
                                      const double min_threshold=10,
                                      const double max_threshold=100);
   void test();

private:
   int                        lines_to_pick_;
   std::string                filename_;
   Date                       observation_start_;
   int                        datastream_;
   std::vector<WeatherRecord> data_; /* all the main stuff */
   std::vector<std::string>   target_;
   std::vector<std::string>   target_names_;
   std::vector<std::string>   feature_names_;
   std::vector<Station>       station_data_;

   bool load();
   void set_target_names();
   void set_station_data();
   void set_feature_names();

   size_t feature_index(const std::string str) const;
   double relative_time(const WeatherRecord& f) const;

   static double get_distance(const Coords& c1,
                              const Coords& c2);
   static Coords get_new_coords(const Coords& c,
                                const double distance,
                                const double bearing);
   static double rad(const double deg) { return deg * M_PI / 180.; }
   static double deg(const double rad) { return rad * 180. / M_PI; }
};

#endif