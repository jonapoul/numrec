#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../global.h"
#include "Feature.h"

int Feature::counter = 0;

Feature::Feature()
         : id_(0),
           name_(""),
           elevation_(0),
           latitude_(0),
           longitude_(0),
           date_(1,1,1),
           time_(0),
           gust_(0),
           temp_(0),
           visibility_(0),
           direction_(""),
           speed_(0),
           pressure_(0),
           tendency_(""),
           dewpoint_(0),
           humidity_(0),
           type_(0) { }

Feature::Feature(const Feature& f)
         : id_(f.id_),
           name_(f.name_),
           elevation_(f.elevation_),
           latitude_(f.latitude_),
           longitude_(f.longitude_),
           date_(f.date_),
           time_(f.time_),
           gust_(f.gust_),
           temp_(f.temp_),
           visibility_(f.visibility_),
           direction_(f.direction_),
           speed_(f.speed_),
           pressure_(f.pressure_),
           tendency_(f.tendency_),
           dewpoint_(f.dewpoint_),
           humidity_(f.humidity_),
           type_(f.type_) { }

Feature::Feature(const std::string line) {
   std::stringstream ss(line);
   ss >> this->id_         >> this->name_      >> this->elevation_   
      >> this->latitude_   >> this->longitude_ >> this->date_ 
      >> this->time_       >> this->gust_      >> this->temp_ 
      >> this->visibility_ >> this->direction_ >> this->speed_
      >> this->pressure_   >> this->tendency_  >> this->dewpoint_ 
      >> this->humidity_   >> this->type_;

   /* only updating counter when this constructor is called */
   index_ = counter++;
}

void Feature::print() const {
   printf("%5d %5d %29s %5.1f %7.3f %7.3f %s %4.0f %6.0f %8.1f %6.0f ",
          index_, id_, name_.c_str(), elevation_, latitude_, longitude_, 
          date_.str().c_str(), time_, gust_, temp_, visibility_);
   printf("%6s %6.0f %6.0f %6s %8.1f %8.1f %1d\n",
          direction_.c_str(), speed_, pressure_, tendency_.c_str(), 
          dewpoint_, humidity_, type_);
}