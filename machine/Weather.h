#ifndef WEATHER_H
#define WEATHER_H

#include <string>
#include <math.h>
#include <vector>
#include <armadillo>

#include "Date.h"
#include "Station.h"
#include "DataPoint.h"
#include "Coords.h"

enum { FULL, ADVANCED, BASIC }; // datastream type

class Weather {
public:
   Weather(const std::string& file,
           const int lines);
   void test();
   arma::mat matrix() const;

   /* getters */
   Date start_date() const;
   int datastream() const;
   size_t num_entries() const;
   size_t num_targets() const;
   size_t num_features() const;
   size_t num_stations() const;
   std::string filename() const;
   std::vector<std::string> target_names() const;
   std::vector<std::string> feature_names() const;

   /* grab values for a given feature/station */
   std::vector<std::string> feature_data(const std::string& feature_name) const;
   std::vector<Station> station_data(const std::string& station) const;

   /* apply various operations on the weather records */
   void fix();
   void modify(const std::string& feature,
               const std::vector<std::string>& values);
   void append(const std::string& name,
               const std::vector<std::string>& values);
   void select(std::vector<std::string> selected_features);
   void discard();
   void delete_feature(const std::string& feature);
   std::vector<DataPoint> get_observations(const std::string& id="" ,
                                               const std::string& date="",
                                               const std::string& time="",
                                               const std::vector<std::string>& features={});
   std::vector<Station> find_stations(const Coords& origin,
                                      const double offset_dist,
                                      const double offset_theta,
                                      const double min_threshold=10,
                                      const double max_threshold=100);

private:
   int                        lines_to_pick_;
   std::string                filename_;
   Date                       observation_start_;
   int                        datastream_;
   std::vector<DataPoint> data_; /* most of the useful data here */
   std::vector<std::string>   target_;
   std::vector<std::string>   target_names_;
   std::vector<std::string>   feature_names_;
   std::vector<Station>       station_data_;

   /* initialisation functions */
   bool load();
   void set_target_names();
   void set_station_data();
   void set_feature_names();

   /* other things */
   size_t feature_index(const std::string str) const;
   double relative_time(const DataPoint& f) const;

   static double get_distance(const Coords& c1,
                              const Coords& c2);
   static Coords get_new_coords(const Coords& c,
                                const double distance,
                                const double bearing);
   static double rad(const double deg) { return deg * M_PI / 180.; }
   static double deg(const double rad) { return rad * 180. / M_PI; }
};

#endif