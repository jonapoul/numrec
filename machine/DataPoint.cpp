#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../global.h"
#include "Date.h"
#include "DataPoint.h"

using std::string;
using std::vector;

int DataPoint::counter = 0;

DataPoint::DataPoint() { /* blank */ }

DataPoint::DataPoint(const DataPoint& wr) { 
   this->features_ = vector<string>(wr.num_features());
   for (size_t i = 0; i < this->num_features(); i++) {
      this->features_[i] = wr.features_[i];
   }
   this->index_ = wr.index_;
}

DataPoint::DataPoint(const string& line) {
   std::stringstream ss(line);
   string temp;
   while (ss >> temp) { 
      this->features_.push_back(temp);
   }
   this->index_ = counter++;
}

void DataPoint::append(const string& value) {
   this->features_.push_back(value);
}

string& DataPoint::operator[](const size_t i) {
   ASSERT( i >= 0 && i < num_features() );
   return this->features_[i];
}

const string& DataPoint::operator[](const size_t i) const {
   ASSERT( i >= 0 && i < num_features() );
   return this->features_[i];
}

void DataPoint::erase(const size_t index) {
   this->features_.erase(this->features_.begin() + index);
}

void DataPoint::empty() {
   this->features_ = {};
}

string DataPoint::str() const {
   std::stringstream ss;
   ss << *this;
   return ss.str();
}

void DataPoint::print() const {
   std::cout << *this << '\n';
}

std::ostream& operator<<(std::ostream& os, const DataPoint& d) {
   os << "i=" << d.index_ << ", [ ";
   for (size_t i = 0; i < d.num_features(); i++) {
      os << d.features_[i] << ' ';
   }
   return os << ']';
}