#include <string>

#include "Date.h"
#include "../global.h"

Date::Date()
         : y_(0), m_(0), d_(0) { }

Date::Date(const Date& that) {
   this->d_ = that.d_;
   this->m_ = that.m_;
   this->y_ = that.y_;
}

Date::Date(const int yy,
           const int mm,
           const int dd)
         : y_(yy), m_(mm), d_(dd) {
   ASSERT( this->d_ > 0 );
   ASSERT( this->m_ > 0 );
   ASSERT( this->y_ > 0 );
   this->balance();
}

Date::Date(const std::string& str) {
   this->y_ = std::stoi(str.substr(0, 4));
   this->m_ = std::stoi(str.substr(5, 2));
   this->d_ = std::stoi(str.substr(8, 2));
   this->balance();
}

void Date::balance() {
   int days[] = { 31, 28, 31, 30, 31, 30, 
                  31, 31, 30, 31, 30, 31 };
   if (is_leap_year(y_)) days[1] = 29;

   while (d_ < 1) {
      d_ += (m_ == 1 ? days[11] : days[m_-2]);
      m_--;
      if (m_ < 1) m_ = 12;
   }
   while (d_ > days[m_-1]) {
      d_ -= days[m_-1];
      m_++;
      if (m_ > 12) m_ = 1;
   }
   while (m_ < 1) {
      m_ += 12;
      y_--;
   }
   while (m_ > 12) {
      m_ -= 12;
      y_++;
   }
}

std::string Date::str(const char separator,
                      const bool use_ddmmyy) const {
   char buf[20];
   if (use_ddmmyy) {
      snprintf(buf, 20, "%02d%c%02d%c%4d", 
               this->d_, separator, this->m_, separator, this->y_);
   } else {
      snprintf(buf, 20, "%4d%c%02d%c%02d", 
               this->y_, separator, this->m_, separator, this->d_);
   }
   return std::string(buf);
}

bool Date::is_leap_year(const int yy) {
   return ( (yy % 4 == 0 && yy % 100 != 0) || yy % 400 == 0 );
}

Date Date::operator+(const Date& that) const {
   Date result(*this);
   result += that;
   result.balance();
   return result;
}

Date Date::operator+(const int days) const {
   Date result(*this);
   result.d_ += days;
   result.balance();
   return result;
}

Date Date::operator-(const Date& that) const {
   Date result(*this);
   result -= that;
   result.balance();
   return result;
}

Date Date::operator-(const int days) const {
   Date result(*this);
   result.d_ -= days;
   result.balance();
   return result;
}

Date& Date::operator+=(const Date& that) {
   this->d_ += that.d_;
   this->m_ += that.m_;
   this->y_ += that.y_;
   this->balance();
   return *this;
}

Date& Date::operator+=(const int days) {
   this->d_ += days;
   this->balance();
   return *this;
}

Date& Date::operator-=(const Date& that) {
   this->d_ -= that.d_;
   this->m_ -= that.m_;
   this->y_ -= that.y_;
   this->balance();
   return *this;
}

Date& Date::operator-=(const int days) {
   this->d_ -= days;
   this->balance();
   return *this;
}

Date Date::operator++() { /* postfix */
   Date result(*this);
   result.d_++;
   result.balance();
   return result;
}


Date Date::operator--() {
   Date result(*this);
   result.d_--;
   result.balance();
   return result;
}

bool Date::operator==(const Date& that) const {
   return ( this->y_ == that.y_ && 
            this->m_ == that.m_ && 
            this->d_ == that.d_ );
}

bool Date::operator!=(const Date& that) const {
   return !(*this == that);
}

bool Date::operator>(const Date& that) const {
   if (this->y_ > that.y_) {
      return true;
   } else if (this->y_ == that.y_) {
      if (this->m_ > that.m_) {
         return true;
      } else if (this->m_ == that.m_) {
         return (this->d_ > that.d_);
      } else {
         return false;
      }
   } else {
      return false;
   }
}

bool Date::operator<(const Date& that) const {
   if (this->y_ < that.y_) {
      return true;
   } else if (this->y_ == that.y_) {
      if (this->m_ < that.m_) {
         return true;
      } else if (this->m_ == that.m_) {
         return (this->d_ < that.d_);
      } else {
         return false;
      }
   } else {
      return false;
   }
}

bool Date::operator>=(const Date& that) const {
   return !(*this < that);
}

bool Date::operator<=(const Date& that) const {
   return !(*this > that);
}

std::istream& operator>>(std::istream& stream, Date& date) {
   std::string str;
   stream >> str;
   date = Date(str);
   return stream;
}

std::ostream& operator<<(std::ostream& os, const Date& d) {
   return os << d.str();
}