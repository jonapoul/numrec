#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <ctime>
#include <iomanip>

#include "../global.h"
#include "Weather.h"
#include "Date.h"
#include "Feature.h"

Weather::Weather(const std::string& file) 
         : filename_(file) {
   /* load data from file and make sure it"s valid */
   ASSERT( this->load() );

   /* add data descriptions */
   this->set_target_names();
   this->set_feature_names();
   this->set_station_data();

   /* set member data defaults */
   this->observation_start_ = Date(2017, 10, 23);
}

Weather::~Weather() {
   delete[] data_;
}

bool Weather::load() {
   std::ifstream filestream(this->filename_);
   ASSERT( filestream.is_open() );

   /* count how many Features we"re reading in */
   this->data_length_ = 0;
   std::string line;
   while (getline(filestream, line)) 
      this->data_length_++;
   filestream.clear();
   filestream.seekg(0, std::ios::beg);
   this->data_length_ -= 1; // account for the first line (full/basice/etc)
   this->data_ = new Feature[this->data_length_];

   /* grab the datastream type */
   getline(filestream, line);
   if      (line == "FULL")     this->datastream_ = 0;
   else if (line == "ADVANCED") this->datastream_ = 1;
   else if (line == "BASIC")    this->datastream_ = 2;
   else                         return false;

   /* construct the Features from each line */
   for (int i = 0; getline(filestream, line); i++) {
      data_[i] = Feature(line);
   }
   return true;
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
   const double phi1 = c1.lat * M_PI / 180.0;
   const double phi2 = c2.lat * M_PI / 180.0;
   const double dPhi = phi2 - phi1;
   const double dLambda = (c2.lon - c1.lon) * M_PI / 180.0;
   const double a = sin(dPhi/2.0)*sin(dPhi/2.0) + 
                    cos(phi1)*cos(phi2)*sin(dLambda/2.0)*sin(dLambda/2.0);
   const double c = 2.0 * atan2( sqrt(a), sqrt(1.0-a) );
   return (R * c)/1000.0;
}

Coords Weather::get_new_coords(const Coords& c,
                               const double distance,    /* km */
                               const double direction) { /* degrees */
   const double magnitude = distance / 6371e3;
   const double angle = (360 - direction) * M_PI / 180.0;
   
   /* http://www.edwilliams.org/avform.htm#LL */
   Coords result;
   result.lat = asin( sin(c.lat)*cos(magnitude) + 
                      cos(c.lat)*sin(magnitude)*cos(angle) );
   result.lon = c.lon - asin(sin(angle)*sin(magnitude)/cos(c.lat)) + M_PI;
   while (result.lon > 2*M_PI) result.lon -= 2*M_PI;
   result.lon -= M_PI;
   return result;
}

double Weather::get_relative_time(const Feature& f) const {

   struct std::tm tm_f;
   std::istringstream iss_f(f.date().str());
   iss_f >> std::get_time(&tm_f,"%Y/%m/%d");
   std::time_t t_f = mktime(&tm_f);
   t_f += 60 * f.time();

   struct std::tm tm_start;
   std::istringstream iss_start(this->observation_start_.str());
   iss_start >> std::get_time(&tm_start,"%Y/%m/%d");
   std::time_t t_start = mktime(&tm_start);

   const double dt = t_f - t_start; // in seconds
   return dt / 3600.0; 
}