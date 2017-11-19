#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <ctime>
#include <algorithm>
#include <iomanip>

#include "../global.h"
#include "Weather.h"
#include "Date.h"
#include "WeatherRecord.h"
#include "Coords.h"
#include "Station.h"

using std::string;
using std::vector;

Weather::Weather(const string& file,
                 const int lines) {
   /* load data from file and make sure it's valid */
   this->lines_to_pick_ = lines;
   this->filename_ = file;
   ASSERT( this->load() );

   /* add data descriptions */
   this->set_target_names();
   this->set_feature_names();
   this->set_station_data();

   /* set member data defaults */
   this->observation_start_ = Date(2017, 10, 23);
}

bool Weather::load() {
   std::ifstream filestream(this->filename_);
   ASSERT( filestream.is_open() );

   /* count how many WeatherRecords we're reading in to allocate space */
   size_t line_count = 0;
   string line;
   while (getline(filestream, line)) 
      line_count++;
   filestream.clear();
   filestream.seekg(0, std::ios::beg);
   line_count -= 1; // account for the first line (FULL/BASIC/ADVANCED)
   
   /* if we've specified how many lines to read, use that instead */
   if (this->lines_to_pick_ > 0) 
      line_count = this->lines_to_pick_;

   this->data_ = vector<WeatherRecord>(line_count);

   /* grab the datastream type */
   getline(filestream, line);
   if      (line == "FULL")     this->datastream_ = 0;
   else if (line == "ADVANCED") this->datastream_ = 1;
   else if (line == "BASIC")    this->datastream_ = 2;
   else                         return false;

   /* construct the WeatherRecords from each line */
   for (int i = 0; getline(filestream, line) && i < (int)line_count; i++) {
      data_[i] = WeatherRecord(line);
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
      target_names_ = { "Clear",  "Partly Cloudy", "Mist",    "Fog",
                        "Cloudy", "Overcast",      "Rain",    "Sleet",
                        "Hail",   "Snow",          "Thunder" };
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
         const Coords coords(data_[i][lat_index], 
                             data_[i][lon_index]);
         const Station s(data_[i][id_index], 
                         data_[i][name_index],
                         coords);
         this->station_data_.push_back( s );
      }
   }
}

void Weather::set_feature_names() {
   this->feature_names_ = { "Station ID", "Station Name", "Elevation", 
                            "Latitude", "Longitude", "Date", 
                            "Time since midnight", "Gust", "Temperature", 
                            "Visibilty", "Wind Direction", "Wind Speed", 
                            "Pressure", "Pressure Trend", "Dew Point", 
                            "Humidity", "Weather Type" };
}

/* distance in km between two points on spherical surface, from 
   https://www.movable-type.co.uk/scripts/latlong.html */
double Weather::get_distance(const Coords& c1,
                             const Coords& c2) {
   const double R = 6371e3;
   const double phi1 = rad(c1.latitude);
   const double phi2 = rad(c2.latitude);
   const double dPhi = phi2 - phi1;
   const double dLambda = rad(c2.longitude - c1.longitude);
   const double a = sin(dPhi/2.)*sin(dPhi/2.) + cos(phi1)*cos(phi2)*sin(dLambda/2.)*sin(dLambda/2.);
   const double c = 2. * atan2( sqrt(a), sqrt(1.-a) );
   return (R * c)/1000.;
}

/* find the coordinates of a point on a sphere at a certain distance and
   bearing from c */
Coords Weather::get_new_coords(const Coords& c,
                               const double distance,  /* km */
                               const double bearing) { /* degrees */
   const double R = 6371.;
   const double oldlat = rad(c.latitude);
   const double oldlon = rad(c.longitude);
   const double magnitude = distance / R;
   const double direction = rad(360 - bearing);
   
   /* http://www.edwilliams.org/avform.htm#LL */
   double lat = asin( sin(oldlat)*cos(magnitude) + 
                      cos(oldlat)*sin(magnitude)*cos(direction));
   double lon = oldlon - asin(sin(direction)*sin(magnitude)/cos(lat)) + M_PI;
   while (lon > 2*M_PI) lon -= 2*M_PI;
   lon -= M_PI;
   return Coords(deg(lat), deg(lon));
}

/* time between observation_start and wr.time in decimal hours */
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

   return (t_wr - t_start) / 3600.0;
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

/* pick out every unique station and their ID/name/coordinates */
vector<Station> Weather::station_data(const string& station) const {
   if (station == "all") {
      return this->station_data_;
   } else {
      vector<string> result;
      for (size_t i = 0; i < this->num_stations(); i++) {
         if (   this->station_data_[i].name == station
             || this->station_data_[i].id   == station) {
            return { this->station_data_[i] };
         }
      }
      printf("ERROR: Nothing matches '%s' in station_data()\n", station.c_str());
      exit(1);
      return {};
   }
}

/* change the values a chosen feature in every record */
void Weather::modify(const string& feature, 
                     const vector<string>& values) {
   ASSERT( num_entries() == values.size() );
   const size_t index = feature_index(feature);
   for (size_t i = 0; i < num_entries(); i++) {
      this->data_[i][index] = values[i];
   }
}

/* add a new feature to every record */
void Weather::append(const string& name,
                     const vector<string>& values) {
   ASSERT( num_entries() == values.size() );
   this->feature_names_.push_back(name);
   for (size_t i = 0; i < num_entries(); i++) {
      this->data_[i].append(values[i]);
   }
}

/* Only keep a selected group of features from all records */
void Weather::select(vector<string> selected_features) {
   const string weather_type = "Weather Type";
   if (!is_in_array(weather_type, selected_features)) {
      selected_features.push_back(weather_type);
   }

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

/* throw away any records with any invalid features */
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
      if (save_this_record) temp_records.push_back(this->data_[i]);
   }
   this->data_ = temp_records;
}

/* delete all of a certain feature from all records */
void Weather::delete_feature(const string& feature) {
   if (is_in_array(feature, this->feature_names_)) {
      const size_t index = feature_index(feature);
      this->feature_names_.erase(this->feature_names_.begin() + index);
      for (size_t i = 0; i < this->num_entries(); i++) {
         this->data_[i].erase(index);
      }
   }
}

/* export object to pickle file */
void Weather::export_to_file(const string& fname) {
   /* get rid of invalid records */
   this->discard();
   /* target = last column */
   this->target_ = feature_data("Weather Type");
   /* remove non-exportable features */
   const vector<string> nonexportable = { "Station ID", "Station Name", 
                                          "Date", "Weather Type" };
   for (auto name : nonexportable) {
      if (is_in_array(name, feature_names_)) {
         this->delete_feature(name);
      }
   }
   /*
      NOW DUMP IT?
      PICKLE.DUMP IS A WAY OF STORING A NUMPY LIST AND THEN PICK IT UP FROM 
      ANOTHER PROGRAM, PROBABLY NOT NECESSARY HERE
   */
}

vector<WeatherRecord> Weather::get_observations(const string& id,
                                                const string& date,
                                                const string& time,
                                                const vector<string>& features) {
   bool id_valid = false, date_valid = false, time_valid = false;
   const size_t id_index   = feature_index("Station ID");
   const size_t date_index = feature_index("Date");
   const size_t time_index = feature_index("Time since midnight");
   if (id.length()   > 0) id_valid   = true; 
   if (date.length() > 0) date_valid = true;
   if (time.length() > 0) time_valid = true;


   vector<WeatherRecord> stats = this->data_;
   /* erase all records with the wrong station id, wrong date or wrong time */
   for (int i = (int)stats.size()-1; i >= 0; i--) {
      if (   (id_valid   && stats[i][id_index]   != id) 
          || (date_valid && stats[i][date_index] != date)
          || (time_valid && stats[i][time_index] != time) ) {
         stats.erase(stats.begin() + i);
      }
   }

   /* only keep features that we chose from the remaining records */
   if (features.size() > 0) {
      vector<int> indices(features.size());
      for (int i = 0; i < (int)indices.size(); i++) {
         if (is_in_array(features[i], feature_names_)) {
            indices[i] = feature_index(features[i]);
         }
      }
      for (auto& s : stats) {
         for (int i = (int)s.num_features(); i >= 0; i--) {
            if (!is_in_array(i, indices)) {
               s.erase(i);
            }
         }
      }
   } 
   return stats;
}

vector<Station> Weather::find_stations(const Coords& origin,
                                       const double offset_dist,
                                       const double offset_theta,
                                       const double min_threshold,
                                       const double max_threshold) {
   const Coords updated = this->get_new_coords(origin, 
                                               offset_dist,
                                               offset_theta);
   vector<Station> nearby_stations;
   for (auto s : this->station_data_) {
      const Coords station_coords(s.coords);
      const double distance = this->get_distance(updated, station_coords);
      if (distance < max_threshold && distance > min_threshold) {
         s.distance = distance;
         nearby_stations.push_back(s);
      }
   }
   auto station_sort = [](const Station& s1, const Station& s2) {
      return (s1.distance < s2.distance);
   };
   std::sort(nearby_stations.begin(), nearby_stations.end(), station_sort);
   return nearby_stations;
}

void Weather::test() {
   printf("###################################\n");
   printf("     Step 3: Inspect the data:\n");
   printf("###################################\n");

   /* print all entries */
   //for (const auto r : this->data_) r.print();

   /* print info about initial data records */
   printf("\nNumber of entries  = %zu\n", this->num_entries());
   printf("Number of targets  = %zu\n", this->num_targets());
   printf("Target Names:\n");
   for (auto t : this->target_names_) std::cout << '\t' << t << '\n';

   printf("\nNumber of features = %zu\n", this->num_features());
   printf("Feature Names:\n");
   for (auto f : this->feature_names_) std::cout << '\t' << f << '\n';

   /* print all station data */
   printf("\nNumber of weather stations = %zu\n", this->num_stations());
   printf("[ ID, Name, Latitude, Longitude, Distance ]\n");
   const auto stations = this->station_data("all");
   for (const auto s : stations) s.print();

   /* grab data about just edinburgh station */
   printf("\nStation data for EDINBURGH/GOGARBANK:\n");
   const auto edinburgh = this->station_data("EDINBURGH/GOGARBANK");
   for (const auto e : edinburgh) e.print();

   /* same for id = 3225 */
   printf("\nStation data for ID 3225:\n");
   const auto station3225 = this->station_data("3225");
   for (const auto s : station3225) s.print();

   printf("\nTemperature data:\n");
   const auto temp_data = this->feature_data("Temperature");
   for (const auto t : temp_data) printf("%5s ", t.c_str());

   printf("\n\n######################################\n");
   printf(" Step 4 - Recovering Incomplete Data\n");
   printf("######################################\n");
   /* since most invalid values are in Gust, reset any invalid values in
      there from -99999 to 0 */
   auto new_gust = this->feature_data("Gust");
   for (auto& g : new_gust) { if (g == "-99999") g = "0"; }
   this->modify("Gust", new_gust);
   //for (const auto r : this->data_) r.print();
   printf("Done!\n");

   printf("\n######################################\n");
   printf("  Step 5 - Removing Incomplete Data\n");
   printf("######################################\n");
   /* delete any remaining datapoints with invalid features */
   this->discard();
   printf("Done!\n");

   printf("\n######################################\n");
   printf("       Step 6 - Data Conversion\n");
   printf("######################################\n");
   /* part 1: Enumerate Pressure Tendency (falling=-1, static=0, rising=+1) */
   auto tendency = this->feature_data("Pressure Trend");
   for (auto& t : tendency) {
      switch (t[0]) {
         case 'F': t = "-1"; break;
         case 'S': t =  "0"; break;
         case 'R': t =  "1"; break;
      }
   }
   this->modify("Pressure Trend", tendency);
   
   /* part 2: Enumerate Wind direction using 16 point compass index */
   auto direction = this->feature_data("Wind Direction");
   for (auto& d : direction) {
      if      (d == "NNE") d = "0";
      else if (d == "NE")  d = "1";
      else if (d == "ENE") d = "2";
      else if (d == "E")   d = "3";
      else if (d == "ESE") d = "4";
      else if (d == "SE")  d = "5";
      else if (d == "SSE") d = "6";
      else if (d == "S")   d = "7";
      else if (d == "SSW") d = "8";
      else if (d == "SW")  d = "9";
      else if (d == "WSW") d = "10";
      else if (d == "W")   d = "11";
      else if (d == "WNW") d = "12";
      else if (d == "NW")  d = "13";
      else if (d == "NNW") d = "14";
      else if (d == "N")   d = "15";
   }
   this->modify("Wind Direction", direction);
   printf("Done!\n");

   printf("\n######################################\n");
   printf("       Step 7 - Data Extraction\n");
   printf("######################################\n");

}