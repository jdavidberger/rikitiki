/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "time_utils.h"

int Date::val() const {
  return 
    10000 * Year + 
    100   * Month +  
    1     * Day;
}

Date::Date(const time_t& t) {
  tm *tm1 = gmtime( &t );
  Year = 1900 + tm1->tm_year;
  Month = tm1->tm_mon + 1;
  Day = tm1->tm_mday;  
}

Date::Date() {
  Month = Day = 0;
  Year = 0;
}

Date::Date(int _Year, char _Month, char _Day) : 
  Year(_Year), Month(_Month), Day(_Day) {
  
}

bool Date::HasValue() const {
  return 
    Year != 0 ||
    Month != 0 ||
    Day != 0;
}

Date Date::Today(){
      time_t t;
      time(&t);
      return Date(t);
}

bool Date::operator>=(const Date& d) const{
  return val() >= d.val();
}

Date::operator Time() const{
  tm timeinfo;
  memset(&timeinfo, 0, sizeof(tm));
  timeinfo.tm_year = Year - 1900;
  timeinfo.tm_mon = Month - 1;
  timeinfo.tm_mday = Day;
  return Time(mktime(&timeinfo));
}
#include <iostream>
std::string Date::YYYYMMDD() const {
  std::string rtn;
  rtn.resize(8);

  rtn[0] = '0' + (Year / 1000) % 10;
  rtn[1] = '0' + (Year / 100) % 10;
  rtn[2] = '0' + (Year / 10) % 10;
  rtn[3] = '0' + (Year / 1) % 10;
  rtn[4] = '0' + (Month / 10) % 10;
  rtn[5] = '0' + (Month / 1) % 10;
  rtn[6] = '0' + (Day / 10) % 10;
  rtn[7] = '0' + (Day / 1) % 10;

  return rtn;
}

Time::Time() : _time(0) {
  
}

Time::Time(const time_t& t) : _time(t) {
  
}

Time Time::Now(){
    Time rtn;
    time(&rtn._time);
    return rtn;
}

Time::operator Date(){
  tm* tm1 = gmtime(&_time);
  return Date(1900 + tm1->tm_year,
	      tm1->tm_mon + 1,
	      tm1->tm_mday);  
}

std::ostream& operator <<(std::ostream& out, const Date& d){
  switch(d.Month){
  case 1: out << "January"; break;
  case 2: out << "February"; break;
  case 3: out << "March"; break;
  case 4: out << "April"; break;
  case 5: out << "May"; break;
  case 6: out << "June"; break;
  case 7: out << "July"; break;
  case 8: out << "August"; break;
  case 9: out << "September"; break;
  case 10: out << "October"; break;
  case 11: out << "November"; break;
  case 12: out << "December"; break;
  default:
    assert("invalid month given" && false);
  }
  return out << " " << d.Day << ", " << d.Year;  
}

