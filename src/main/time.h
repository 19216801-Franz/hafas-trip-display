#ifndef TIME_H
#define TIME_H

#include <WiFiUdp.h>
#include <NTPClient.h>

void init_time();
String format_date(struct tm* t);
int get_day();
int get_month();
int get_year();
int get_hour();
int get_mins();
int get_secs();
#endif
