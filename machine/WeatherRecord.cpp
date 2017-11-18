#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../global.h"
#include "Date.h"
#include "WeatherRecord.h"

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

void WeatherRecord::print() const {
   printf("%5d ", index_);
   for (size_t i = 0; i < num_features(); i++) {
      printf("%s ", features_[i].c_str());
   }
   printf("\n");
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

vector<string>::iterator WeatherRecord::begin() {
   return features_.begin();
}

void WeatherRecord::erase(vector<string>::iterator beg) {
   features_.erase(beg);
}