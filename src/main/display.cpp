#include "include/display.h"

GxIO_Class spi(SPI, PIN_CS, PIN_DC, PIN_RST);
GxEPD_Class display(spi, PIN_RST, PIN_BUSY);

String replace_umlaut(String s){
  s.replace(String("ä"), String("ae"));
  s.replace(String("ö"), String("oe"));
  s.replace(String("ü"), String("ue"));
  return s;
}

void erase_display(){  
  display.eraseDisplay();
}

void update_display(){
  display.update();
}

void init_display(){
  Serial.println("Initialising display...");
  display.init();
  display.setTextColor(GxEPD_BLACK);
  // most e-papers have width < height (portrait) as native orientation, especially the small ones
  // in GxEPD rotation 0 is used for native orientation (most TFT libraries use 0 fix for portrait orientation)
  // set rotation to 1 (rotate right 90 degrees) to have enough space on small displays (landscape)
  //display.setRotation(1);
  //TODO: Wrong
  display.setRotation(3);
}

void print_red(String s){
  display.setTextColor(GxEPD_RED);
  display.print(s);
  display.setTextColor(GxEPD_BLACK);
}

void upper(leg &l){
  char arrival[6];
  display.setFont(&FONT_SMALL);
  display.setCursor(0,5);
  display.print("--------------------------------------------");
  display.setCursor(0, display.getCursorY() + 28);
  display.setFont(&FONT_BIG);

  // Remove 'Bus' from name string
  String name = l.name;
  if (name.indexOf("Bus ") == 0){
    name = name.substring(4);
  }
  
  String dest = l.direction;
  if (dest.indexOf("Kiel ") == 0){
    dest = dest.substring(5);
  }

  dest = replace_umlaut(dest);

  // In case of no delay, scheduled_arrival == delayed_arrival
  boolean overnight = l.delayed_arrival.tm_mday != get_day();
  int hours = l.delayed_arrival.tm_hour;
  int mins = l.delayed_arrival.tm_min;
  sprintf(arrival, "%02i:%02i", hours, mins);

  //Serial.printf("Printing: name %s\ndest %s\nhours %i\nmins %i\novernight %s\n", name, dest, hours, mins, overnight);
  
  print_red("["); display.print(name); print_red("]"); display.print(" at ");
  if (l.has_delay){
    print_red(arrival);
  } else {
    display.print(arrival);
  }
  display.setFont(&FONT_MIDDLE);
  display.print("\n");
  display.print(String("-> ") + dest);
  display.setFont(&FONT_SMALL);
  display.print("\n");
  display.print("--------------------------------------------");
  //TODO: Richtiges Updatewindow finden
  //display.updateWindow(50,50,50,100); 
}

void lower(leg &l){
  char arrival[6];
  display.setCursor(0, display.getCursorY() + 28);
  display.setFont(&FONT_BIG);

  // Remove 'Bus' from name string
  String name = l.name;
  if (l.name.indexOf("Bus ") == 0){
    name = l.name.substring(4);
  }
  String dest = l.direction;
  if (l.direction.indexOf("Kiel ") == 0){
    dest = l.direction.substring(5);
  }

  // In case of no delay, scheduled_arrival == delayed_arrival
  boolean overnight = l.delayed_arrival.tm_mday != get_day();
  int hours = l.delayed_arrival.tm_hour;
  int mins = l.delayed_arrival.tm_min;
  sprintf(arrival, "%02i:%02i", hours, mins);

  //Serial.printf("Printing: name %s\ndest %s\nhours %i\nmins %i\novernight %s\n", name, dest, hours, mins, overnight);
  
  print_red("["); display.print(name); print_red("]"); display.print(" at ");
  if (l.has_delay){
    print_red(arrival);
  } else {
    display.print(arrival);
  }
  display.setFont(&FONT_MIDDLE);
  display.print("\n");
  display.print(String("-> ") + dest);
  display.setFont(&FONT_SMALL);
  display.print("\n");
  display.print("--------------------------------------------");
  //TODO: Richtiges Updatewindow finden
  //display.updateWindow(50,50,50,100); 
}

void print_string(String s)
{
  Serial.println("Printing string to display: \"" + s + String("\""));
  display.setFont(&FONT_MIDDLE);
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(0, 18);
  display.print(s);
  display.update();
  display.fillScreen(GxEPD_WHITE);
}