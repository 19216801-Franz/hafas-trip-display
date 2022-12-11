#ifndef EPD_DISPLAY
#define EPD_DISPLAY

#include <GxEPD.h>
#include <GxGDEW027C44/GxGDEW027C44.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <Fonts/cozette6.h>
#include <Fonts/cozette12.h>
#include <Fonts/cozette18.h>

#include "hafas.h"
#include "time.h"

#define FONT_SMALL CozetteVector6pt7b
#define FONT_SMALL_MAX_CHARS 44

#define FONT_MIDDLE CozetteVector12pt7b
#define FONT_MIDDLE_MAX_CHARS 22

#define FONT_BIG CozetteVector18pt7b
#define FONT_BIG_MAX_CHARS 15

// Hardware SPI Interface Esp8266
// CS -> GPIO 15, CLK -> GPIO 14, DIN/MOSI -> GPIO 13
/*CS=D8*//*DC=D3*//*RST=D4*//*RST=D4*//*BUSY=D2*/
#define PIN_CS 15
#define PIN_DC 0
#define PIN_RST 5
#define PIN_BUSY 4

#define BACKGROUND GxEPD_WHITE

void erase_display();
void update_display();
void init_display();
void upper(leg &l);
void lower(leg &l);
#endif
