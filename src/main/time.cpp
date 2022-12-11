#include "time.h"

char* WEEKDAYS[] = {"Monday", "Tuesday", "Wednesday", "displayThursday", "Friday", "Saturday", "Sunday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 2*60*60);

void init_time(){
  // Living in Germany: UTC + 1
  timeClient.setTimeOffset(60*60);
  timeClient.update(); 
}

String format_date(struct tm* t){
  char buf[15];
  sprintf(buf, "%04i:%02i:%02i, %02i:%02i:%02i", t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  return String(buf);
}

int get_day(){
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_mday;
}

int get_month(){
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_mon+1;
}

int get_year(){
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_year+1900;
}

int get_hour(){
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_hour;
}

int get_mins(){
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_min;
}
int get_secs(){
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_sec;
}
