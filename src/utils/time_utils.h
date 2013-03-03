/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <time.h>
#include <string>
#include <string.h>
#include <iostream>
#include <assert.h>

class Time;
class Date;

class Time {
  time_t _time;
 public:
  Time();
  Time(const time_t& t);
  static Time Now();
  operator Date();
};

class Date {
  int val() const;
 public:
  unsigned char Month, Day; 
  int Year;
  static Date Today();
  Date();
  Date(const time_t& time);
  Date(int Year, char Month, char Day);
  bool HasValue() const;
  bool operator>=(const Date& d) const;  
  std::string YYYYMMDD() const;
  operator Time() const;
};

std::ostream& operator <<(std::ostream& out, const Date& d);
