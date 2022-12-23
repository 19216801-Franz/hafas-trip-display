#ifndef MAIN_H
#define MAIN_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <stdarg.h>
#include <EEPROM.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include "time.h"
#include "hafas.h"
#include "display.h"

#define SSID_LEN 32
#define PW_LEN 64
#define MY_SSID "temp-hotspot" //"default gateway"
#define MY_PW "temp-password" //"all_packages_here_pls"
#define TEMP_SSID "temp-hotspot" //Doesnt work on apple
#define TEMP_PW "temp-password"
#define CONNECTION_RETRIES 30
#define SECOND 1000

typedef struct {
    char ssid[SSID_LEN];
    char pw[PW_LEN];
} credentials;

#endif
