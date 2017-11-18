#ifndef WeatherRecord_H
#define WeatherRecord_H

#include <string>
#include <vector>
using std::string;
using std::vector;

class WeatherRecord {
public:
   WeatherRecord();
   WeatherRecord(const WeatherRecord& f);
   WeatherRecord(const string& line);

   void print() const;
   void append(const string& value);
   size_t num_features() const { return features_.size(); }

   string& operator[](const size_t i);
   const string& operator[](const size_t i) const;
   vector<string>::iterator begin();
   void erase(vector<string>::iterator beg);


private:
   static int counter;
   int index_;
   vector<string> features_;
};

#endif