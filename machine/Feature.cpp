#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Feature.h"

Feature::Feature()
         : id_(0),
           name_(""),
           x0_(0),
           latitude_(0),
           longitude_(0),
           date_(0,0,0),
           x3_(0),
           x4_(0),
           x5_(0),
           x6_(0),
           direction_(0),
           x7_(0),
           x8_(0),
           tendency_('a'),
           x10_(0),
           x11_(0),
           type_(0) { }

Feature::Feature(const Feature& f)
         : id_(f.id_),
           name_(f.name_),
           x0_(f.x0_),
           latitude_(f.latitude_),
           longitude_(f.longitude_),
           date_(f.date_),
           x3_(f.x3_),
           x4_(f.x4_),
           x5_(f.x5_),
           x6_(f.x6_),
           direction_(f.direction_),
           x7_(f.x7_),
           x8_(f.x8_),
           tendency_(f.tendency_),
           x10_(f.x10_),
           x11_(f.x11_),
           type_(f.type_) { }

Feature::Feature(const std::string line) {
   std::stringstream ss(line);
   ss >> this->id_ >> this->name_ >> this->x0_ >> this->latitude_ >> this->longitude_;
   std::string datestr;
   ss >> datestr;
   this->date_ = Date(datestr);
   ss >> this->x3_ >> this->x4_ >> this->x5_ >> this->x6_ >> this->x7_;
   ss >> this->direction_ >> this->x7_ >> this->x8_ >> this->tendency_;
   ss >> this->x10_ >> this->x11_ >> this->type_;
}