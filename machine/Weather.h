#ifndef WEATHER_H
#define WEATHER_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "Date.h"
#include "WeatherRecord.h"

struct Coords { 
   double latitude; 
   double longitude; 
   Coords() { latitude = 0; longitude = 0; }
   Coords(const double lat,const double lon) {
      latitude=lat; longitude=lon;
   }
   Coords(const string& lat,const string& lon) {
      latitude = stod(lat); longitude = stod(lon);
   }
};

struct Station {
   string id;
   string name;
   string latitude;
   string longitude;
   Station(const string& ID,const string& n,
           const string& lat,const string& lon) {
      id = ID; name = n; latitude  = lat; longitude = lon;
   }
   void print() const {
      printf("%s %s %s %s\n", id.c_str(), name.c_str(), 
                              latitude.c_str(), longitude.c_str());
   }
};

enum { FULL, ADVANCED, BASIC }; // datastream type

class Weather {
public:
   Weather(const string& file);
   ~Weather();

   string          filename()      const { return filename_; }
   Date            start_date()    const { return observation_start_; }
   int             datastream()    const { return datastream_; }
   size_t          num_entries()   const { return data_.size(); }
   vector<string>  target_names()  const { return target_names_; }
   size_t          num_targets()   const { return target_names_.size(); }
   vector<string>  feature_names() const { return feature_names_; }
   size_t          num_features()  const { return feature_names_.size(); }
   size_t          num_stations()  const { return station_data_.size(); }

   vector<string>  feature_data(const string& feature_name) const;
   vector<Station> station_data(const string& station_name) const;

   void modify(const string& feature, const vector<string>& values);
   void append(const string& name,    const vector<string>& values);
   void select(vector<string> selected_features);
   void discard();
   void delet(const string& feature);

private:
   string                filename_;
   Date                  observation_start_;
   int                   datastream_;
   vector<WeatherRecord> data_;
   vector<string>        target_;
   vector<string>        target_names_;
   vector<string>        feature_names_;
   vector<Station>       station_data_;

   bool load();
   void set_target_names();
   void set_station_data();
   void set_feature_names();

   size_t feature_index(const string str) const;
   double relative_time(const WeatherRecord& f) const;

   static double get_distance(const Coords& c1,
                              const Coords& c2);
   static Coords get_new_coords(const Coords& c,
                                const double distance,
                                const double direction);
};

#endif