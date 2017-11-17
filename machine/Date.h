#ifndef DATE_H
#define DATE_H

#include <string>

class Date {
public:
   Date();
   Date(const Date&);
   Date(const int yy, const int mm, const int dd);
   Date(const std::string& str);

   int year()  const { return y_; }
   int month() const { return m_; }
   int day()   const { return d_; }

   std::string str() const;

   Date  operator+ (const Date&) const;
   Date  operator- (const Date&) const;
   Date& operator+=(const Date&);
   Date& operator-=(const Date&);
   Date  operator+ (const int days) const;
   Date  operator- (const int days) const;
   Date& operator+=(const int days);
   Date& operator-=(const int days);
   Date  operator++();
   Date  operator--();
   bool  operator==(const Date&) const;
   bool  operator!=(const Date&) const;
   bool  operator> (const Date&) const;
   bool  operator< (const Date&) const;
   bool  operator>=(const Date&) const;
   bool  operator<=(const Date&) const;

private:
   int y_;
   int m_;
   int d_;

   void balance();
   static bool is_leap_year(const int yy);
};

#endif