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
  //TODO:
  //display.eraseDisplay();
  display.fillScreen(GxEPD_WHITE);
}

void update_display(){
  display.update();
}

void power_display_up(){
  display.init();
}

void power_display_down(){
  display.powerDown();
}

void init_display(){
  Serial.println("Initialising display...");
  power_display_up();
  display.setTextColor(GxEPD_BLACK);
  // rotation 3 (rotate right 90 degrees 3 times) for reversed landscape
  display.setRotation(3);
}

void print_red(String s){
  display.setTextColor(GxEPD_RED);
  display.print(s);
  display.setTextColor(GxEPD_BLACK);
}

void print_leg_to_display(int16_t start, leg &l){
  Serial.printf("Printing leg at %i", start);
  char arrival[6];
  display.setFont(&FONT_SMALL);
  display.setCursor(0,start);
  display.print("--------------------------------------------");
  display.setCursor(0, display.getCursorY() + 28);
  display.setFont(&FONT_BIG);

  // Remove 'Bus' from name string
  String name = l.name;
  if (name.indexOf("Bus ") == 0){
    name = name.substring(4);
  }
  
  String direction = l.direction;
  if ((direction.length() > (FONT_MIDDLE_MAX_CHARS - 3)) && (direction.indexOf("Kiel ") == 0)){
    direction = direction.substring(5);
  }

  direction = replace_umlaut(direction);

  // In case of no delay, scheduled_arrival == delayed_arrival
  boolean overnight = l.delayed_arrival.tm_mday != get_day();
  int hours = l.delayed_arrival.tm_hour;
  int mins = l.delayed_arrival.tm_min;
  sprintf(arrival, "%02i:%02i", hours, mins);

  print_red("["); display.print(name); print_red("]"); display.print(" at ");
  if (l.has_delay){
    print_red(arrival);
  } else {
    display.print(arrival);
  }
  display.setFont(&FONT_MIDDLE);
  display.print("\n");

  // abbreaveating too long direction strings.
  if (direction.length() > (FONT_MIDDLE_MAX_CHARS - 3)){
    direction = direction.substring(0, FONT_MIDDLE_MAX_CHARS - 3);
     direction.setCharAt(direction.length() - 1, '.');
  }
  display.print(String("-> ") + direction);
  display.setFont(&FONT_SMALL);
  display.print("\n");
  display.print("--------------------------------------------");
  display.updateWindow(0, start, MAX_WIDTH, WINDOW_SIZE);
}

void update_info_window(String s){
  Serial.println("Updating info window");
  display.setCursor(0, SECOND_WINDOW_START + WINDOW_SIZE + 8);
  display.setFont(&FONT_SMALL);
  if (s.equals("")){
    display.print(get_pretty_date());
    display.print("\nUptime: " + get_passed_time());
  } else {
    display.print(s);
  }
  display.updateWindow(0, SECOND_WINDOW_START + WINDOW_SIZE, MAX_WIDTH, MAX_HEIGHT - (SECOND_WINDOW_START + WINDOW_SIZE));
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