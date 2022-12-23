#include "include/time.h"

char* WEEKDAYS[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char* MONTHS[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "Oktober", "November", "December"};
WiFiUDP ntp_udp_client;
time_t start_time;
NTPClient time_client(ntp_udp_client, "pool.ntp.org", 2*60*60);

void init_time(){
  Serial.println("Initialising time...");
  // Living in Germany: UTC + 1
  time_client.setTimeOffset(60*60);
  time_client.update(); 

  start_time = time_client.getEpochTime();

  Serial.println("Time initialised.");
  Serial.printf("It is the %02i:%02i:%04i at %02i:%02i:%02i o' clock.\n", get_day(), get_month(), get_year(), get_hour(),get_mins(),get_secs());
}

String get_dayname(){
  time_t epochTime = time_client.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return String(WEEKDAYS[ptm->tm_wday]);
}

String get_monthname(){
  return String(MONTHS[get_month()-1]);
}

String get_pretty_date(){
  return String("Today is " + get_dayname() + ",\nthe " + String(get_day()) + "th of " + get_monthname() + " " + String(get_year()));
}

String get_passed_time(){
  char buf[27];
  time_t current_time= time_client.getEpochTime();
  long long int passed_time = llround(difftime(current_time, start_time));
  int days = passed_time / (60*60*24);
  int hours = (passed_time / (60*60)) % 24;
  int mins = (passed_time / (60)) % 60;
  sprintf(buf, "%03i days, %02i hours, %02i mins", days, hours, mins);
  return String(buf);
}

String get_date_string(){
  char buf[15];
  sprintf(buf, "%04i:%02i:%02i, %02i:%02i:%02i", get_year(), get_month(), get_day(), get_hour(), get_mins(), get_secs());
  return String(buf);
}

String format_date(struct tm* t){
  char buf[15];
  sprintf(buf, "%04i:%02i:%02i, %02i:%02i:%02i", t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  return String(buf);
}

int get_day(){
  time_t epochTime = time_client.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_mday;
}

int get_month(){
  time_t epochTime = time_client.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  Serial.printf("get_month(): returning: %i", ptm->tm_mon+1);
  return ptm->tm_mon+1;
}

int get_year(){
  time_t epochTime = time_client.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_year+1900;
}

int get_hour(){
  time_t epochTime = time_client.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_hour;
}

int get_mins(){
  time_t epochTime = time_client.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_min;
}
int get_secs(){
  time_t epochTime = time_client.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  return ptm->tm_sec;
}
