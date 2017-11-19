#ifndef DATAPOINT_H
#define DATAPOINT_H

#include <string>
#include <vector>

class DataPoint {
public:
   DataPoint();
   DataPoint(const DataPoint& f);
   DataPoint(const std::string& line);

   void append(const std::string& value);
   size_t num_features() const { return features_.size(); }

   std::string& operator[](const size_t i);
   const std::string& operator[](const size_t i) const;
   std::vector<std::string>::iterator begin();
   void erase(std::vector<std::string>::iterator beg);
   void erase(const size_t index);
   void empty();
   
   std::string str() const;
   void print() const;
   friend std::ostream& operator<<(std::ostream& os, const DataPoint& d);

private:
   static int counter;
   int index_;
   std::vector<std::string> features_;
};

#endif