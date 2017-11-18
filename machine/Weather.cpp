#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <ctime>
#include <iomanip>

#include "../global.h"
#include "Weather.h"
#include "Date.h"
#include "WeatherRecord.h"

Weather::Weather(const string& file) {
   /* load data from file and make sure it's valid */
   this->filename_ = file;
   ASSERT( this->load() );

   /* add data descriptions */
   this->set_target_names();
   this->set_feature_names();
   this->set_station_data();

   /* set member data defaults */
   this->observation_start_ = Date(2017, 10, 23);
}

Weather::~Weather() { /* blank */ }

bool Weather::load() {
   std::ifstream filestream(this->filename_);
   ASSERT( filestream.is_open() );

   /* count how many WeatherRecords we"re reading in to allocate space */
   size_t count = 0;
   string line;
   while (getline(filestream, line)) 
      count++;
   filestream.clear();
   filestream.seekg(0, std::ios::beg);
   count -= 1; // account for the first line (full/basic/etc)
   this->data_ = vector<WeatherRecord>(count);

   /* grab the datastream type */
   getline(filestream, line);
   if      (line == "FULL")     this->datastream_ = 0;
   else if (line == "ADVANCED") this->datastream_ = 1;
   else if (line == "BASIC")    this->datastream_ = 2;
   else                         return false;

   /* construct the WeatherRecords from each line */
   for (int i = 0; getline(filestream, line); i++) {
      data_[i] = WeatherRecord(line);
      data_[i].print();
   }
   return true;
}

/* index of str in the array of features */
size_t Weather::feature_index(const string str) const {
   for (size_t i = 0; i < num_features(); i++) {
      if (feature_names_[i] == str) return i;
   }
   printf("ERROR: input of \"%s\" to feature_index() didn't match\n",
          str.c_str());
   exit(1);
}

void Weather::set_target_names() {
   ASSERT( datastream_ == 0 || datastream_ == 1 || datastream_ == 2 );
   switch (this->datastream_) {
   case 0: 
      target_names_ = { "Clear Night",              "Sunny Day", 
                        "Partly cloudy (night)",    "Partly cloudy (day)", 
                        "Not used",                 "Mist",
                        "Fog",                      "Cloudy", 
                        "Overcast",                 "Light rain shower (night)", 
                        "Light rain shower (day)",  "Drizzle", 
                        "Light rain",               "Heavy rain shower (night)", 
                        "Heavy rain shower (day)",  "Heavy rain", 
                        "Sleet shower (night)",     "Sleet shower (day)", 
                        "Sleet",                    "Hail shower (night)", 
                        "Hail shower (day)",        "Hail", 
                        "Light snow shower (night)","Light snow shower (day)", 
                        "Light snow",               "Heavy snow shower (night)", 
                        "Heavy snow shower (day)",  "Heavy snow", 
                        "Thunder shower",           "Thunder shower (night)", 
                        "Thunder" }; 
      return;
   case 1:
      target_names_ = {"Clear",  "Partly Cloudy", "Mist",    "Fog",
                       "Cloudy", "Overcast",      "Rain",    "Sleet",
                       "Hail",   "Snow",          "Thunder"};
      return;
   case 2:
      target_names_ = { "Clear", "Cloudy", "Precipitation" };
      return;
   }
}

void Weather::set_station_data() {
   this->station_data_ = { };
   vector<string> found_ids = { };

   const size_t id_index   = feature_index("Station ID");
   const size_t name_index = feature_index("Station Name");
   const size_t lat_index  = feature_index("Latitude");
   const size_t lon_index  = feature_index("Longitude");

   for (size_t i = 0; i < num_entries(); i++) {
      if ( !is_in_array(data_[i][id_index], found_ids) ) {
         found_ids.push_back(data_[i][id_index]); 
         this->station_data_.push_back( Station(data_[i][id_index], 
                                                data_[i][name_index], 
                                                data_[i][lat_index], 
                                                data_[i][lon_index]) );
      }
   }
   for (auto s : station_data_) s.print();
}

void Weather::set_feature_names() {
   this->feature_names_ = { "Station ID", "Station Name", "Elevation", 
                            "Latitude", "Longitude", "Date", 
                            "Time since midnight", "Gust", "Temperature", 
                            "Visibilty", "Wind Direction", "Wind Speed", 
                            "Pressure", "Pressure Trend", "Dew Point", 
                            "Humidity", "Weather Type" };
}

/* from https://www.movable-type.co.uk/scripts/latlong.html */
double Weather::get_distance(const Coords& c1,
                             const Coords& c2) {
   const double R = 6371e3;
   const double phi1 = c1.latitude * M_PI / 180.0;
   const double phi2 = c2.latitude * M_PI / 180.0;
   const double dPhi = phi2 - phi1;
   const double dLambda = (c2.longitude - c1.longitude) * M_PI / 180.0;
   const double a = sin(dPhi/2.0)*sin(dPhi/2.0) + 
                    cos(phi1)*cos(phi2)*sin(dLambda/2.0)*sin(dLambda/2.0);
   const double c = 2.0 * atan2( sqrt(a), sqrt(1.0-a) );
   return (R * c)/1000.0;
}

Coords Weather::get_new_coords(const Coords& c,
                               const double distance, /* km */
                               const double angle) {  /* degrees */
   const double magnitude = distance / 6371e3;
   const double theta = (360 - angle) * M_PI / 180.0;
   
   /* http://www.edwilliams.org/avform.htm#LL */
   const double lat = asin( sin(c.latitude)*cos(magnitude) + 
                            cos(c.latitude)*sin(magnitude)*cos(theta) );
   const double argument = sin(theta) * sin(magnitude) / cos(c.latitude);
   double lon = c.longitude - asin(argument) + M_PI;
   while (lon > 2*M_PI) lon -= 2*M_PI;
   lon -= M_PI;
   return Coords(lat, lon);
}

double Weather::relative_time(const WeatherRecord& wr) const {
   struct std::tm tm_wr;
   const string date = wr[ feature_index("Date") ];
   std::istringstream iss_wr(date);
   iss_wr >> std::get_time(&tm_wr,"%Y/%m/%d");
   std::time_t t_wr = mktime(&tm_wr);
   t_wr += 60 * stod( wr[ feature_index("Time since midnight") ] );

   struct std::tm tm_start;
   std::istringstream iss_start(this->observation_start_.str());
   iss_start >> std::get_time(&tm_start,"%Y/%m/%d");
   std::time_t t_start = mktime(&tm_start);

   return (t_wr - t_start) / 3600.0; /* time difference in decimal hours */
}

/* gets all entries for the given feature name
   e.g. feature_name = "Temperature" => returns array of all T values */
vector<string> Weather::feature_data(const string& feature_name) const {
   vector<string> result(num_entries());
   const size_t feat_index = feature_index(feature_name);
   for (size_t i = 0; i < num_entries(); i++) {
      result[i] = data_[i][feat_index];
   }
   return result;
}

/* */
vector<Station> Weather::station_data(const string& station_name) const {
   if (station_name == "all") {
      return this->station_data_;
   } else {
      vector<string> result;
      for (size_t i = 0; i < num_stations(); i++) {
         if (this->station_data_[i].name == station_name) {
            return { this->station_data_[i] };
         }
      }
      printf("ERROR: No stations match the name %s in station_data()\n",
             station_name.c_str());
      exit(1);
      return {};
   }
}

void Weather::modify(const string& feature, 
                     const vector<string>& values) {
   ASSERT( num_entries() == values.size() );
   const size_t index = feature_index(feature);
   for (size_t i = 0; i < num_entries(); i++) {
      this->data_[i][index] = values[i];
   }
}

void Weather::append(const string& name,
                     const vector<string>& values) {
   ASSERT( num_entries() == values.size() );
   this->feature_names_.push_back(name);
   for (size_t i = 0; i < num_entries(); i++) {
      this->data_[i].append(values[i]);
   }
}

void Weather::select(vector<string> selected_features) {
   const string weather_type = "Weather Type";
   if (!is_in_array(weather_type, selected_features))
      selected_features.push_back(weather_type);

   const size_t N_selected = selected_features.size();
   vector<size_t> feature_indices(N_selected);
   for (size_t i = 0; i < N_selected; i++) {
      feature_indices[i] = feature_index(selected_features[i]);
   }
   vector<string> temp_names(N_selected);
   for (size_t i = 0; i < N_selected; i++) {
      temp_names[i] = this->feature_names_[ feature_indices[i] ];
   }
   this->feature_names_ = temp_names;

   for (size_t i = 0; i < num_entries(); i++) {
      WeatherRecord wr;
      for (size_t j = 0; j < N_selected; j++) {
         const size_t index = feature_indices[j];
         wr.append( this->data_[i][index] );
      }
      this->data_[i] = wr;
   }
}

void Weather::discard() {
   vector<WeatherRecord> temp_records;
   for (size_t i = 0; i < num_entries(); i++) {
      bool save_this_record = true;
      for (size_t j = 0; j < this->data_[i].num_features(); j++) {
         if (this->data_[i][j] == "-99999") {
            save_this_record = false;
            break;
         }
      }
      if (save_this_record) {
         temp_records.push_back(this->data_[i]);
      }
   }
   this->data_ = temp_records;
}

void Weather::delet(const string& feature) {
   if (is_in_array(feature, this->feature_names_)) {
      const size_t index = feature_index(feature);
      this->feature_names_.erase(this->feature_names_.begin() + index);
      for (size_t i = 0; i < this->num_entries(); i++) {
         this->data_[i].erase(this->data_[i].begin() + index);
      }
   }
}