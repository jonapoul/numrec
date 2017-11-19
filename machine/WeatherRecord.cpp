#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../global.h"
#include "Date.h"
#include "WeatherRecord.h"

using std::string;
using std::vector;

int WeatherRecord::counter = 0;

WeatherRecord::WeatherRecord() { /* blank */ }

WeatherRecord::WeatherRecord(const WeatherRecord& wr) { 
   this->features_ = vector<string>(wr.num_features());
   for (size_t i = 0; i < this->num_features(); i++) {
      this->features_[i] = wr.features_[i];
   }
   this->index_ = wr.index_;
}

WeatherRecord::WeatherRecord(const string& line) {
   std::stringstream ss(line);
   string temp;
   while (ss >> temp) { 
      this->features_.push_back(temp);
   }
   this->index_ = counter++;
}

void WeatherRecord::append(const string& value) {
   features_.push_back(value);
}

string& WeatherRecord::operator[](const size_t i) {
   ASSERT( i >= 0 && i < num_features() );
   return features_[i];
}

const string& WeatherRecord::operator[](const size_t i) const {
   ASSERT( i >= 0 && i < num_features() );
   return features_[i];
}

void WeatherRecord::erase(const size_t index) {
   features_.erase(features_.begin() + index);
}

string WeatherRecord::str() const {
   std::stringstream ss;
   ss << *this;
   return ss.str();
}

void WeatherRecord::print() const {
   std::cout << *this << '\n';
}

std::ostream& operator<<(std::ostream& os, const WeatherRecord& d) {
   os << d.index_ << ' ';
   for (size_t i = 0; i < d.num_features(); i++) {
      os << d.features_[i] << ' ';
   }
   return os;
}