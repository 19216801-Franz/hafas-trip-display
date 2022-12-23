#ifndef HAFAS_H
#define HAFAS_H

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "time.h"

#define HAFAS_URL "http://nah.sh.hafas.de/bin/mgate.exe"
#define HOME_STATION 9049128
#define LEGS_COUNT 2
#define HTTP_OK 200
#define JSON_BUFFER_SIZE 12000


typedef struct leg{
  struct tm scheduled_arrival;
  String name;
  String direction;
  String station;
  bool has_delay;
  struct tm delayed_arrival;
  bool cancelled;
  } leg;

void print_leg(leg &l);
int parse_legs(StaticJsonDocument<JSON_BUFFER_SIZE> &buffer, leg (&legs)[LEGS_COUNT]);
int update_departures(StaticJsonDocument<JSON_BUFFER_SIZE> &buffer);

#endif
