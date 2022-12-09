#include "main.h"

StaticJsonDocument<JSON_BUFFER_SIZE> buffer;
leg legs[LEGS_COUNT];
GxIO_Class spi(SPI, PIN_CS, PIN_DC, PIN_RST);
GxEPD_Class display(spi, PIN_RST, PIN_BUSY);

void my_panic(){
  for(;;){
    Serial.println("PANIC!");
    delay(1000);
  }
}

void init_wifi(){
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.begin(MY_SSID,MY_PW);

  Serial.print("Connecting");
  for(;;){
    if (WiFi.status() == WL_CONNECTED){
      break;
    }

    delay(2000);
    WiFi.printDiag(Serial);
  }
  
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.setDebugOutput(true);
  Serial.begin(9600);

  init_wifi();
  init_time();

  
  Serial.printf("It is the %04i:%02i:%02i at %02i:%02i:%02i o' clock.\n", get_day(), get_month(), get_year(), get_hour(),get_mins(),get_secs());
  
  Serial.println("fill!");

  display.init(9600);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(3);
  display.setCursor(0,30);
  display.print("HELLO WORLD");
  display.update();
  display.setFont(&FONT_SMALL);
  display.print("--------------------------------------------");
  display.setFont(&FONT_BIG);
  display.print("[51] in 01:48");
  display.setFont(&FONT_MIDDLE);
  display.print("\n");
  display.print("-> Kiel Hauptbahnhof");
  display.setFont(&FONT_SMALL);
  display.update();
  display.print("\n");
  display.print("--------------------------------------------");
  display.setFont(&FONT_BIG);
  display.print("\n");
  display.print("[51] in 01:48");
  display.setFont(&FONT_MIDDLE);
  display.print("\n");
  display.print("-> Kiel Hauptbahnhof");
  display.setFont(&FONT_SMALL);
  display.print("\n");
  display.print("--------------------------------------------");
  display.update();
  Serial.println("fill slot done");
  
  display.powerDown();
}


void loop() {
  delay(3000);
  if(WiFi.status() == WL_CONNECTED){
      if(update_departures(buffer) == 0){
        Serial.println("No departures!");
        return;
      }

     parse_legs(buffer, legs);

     Serial.println("\n\n\nParsed legs\n\n:");
     for( int i = 0; i < LEGS_COUNT; ++i){
      print_leg(legs[i]);
      Serial.println();
     }
    }
    else {
      Serial.println("Lost Wifi Connection!");
    }
}
/*
void drawHelloWorldForDummies()
{
  // This example function/method can be used with full buffered graphics AND/OR paged drawing graphics
  // for paged drawing it is to be used as callback function
  // it will be executed once or multiple times, as many as needed,
  // in case of full buffer it can be called directly, or as callback
  // IMPORTANT: each iteration needs to draw the same, to avoid strange effects
  // use a copy of values that might change, don't read e.g. from analog or pins in the loop!
  //Serial.println("drawHelloWorldForDummies");
  const char text[] = "Hello World!";
  // most e-papers have width < height (portrait) as native orientation, especially the small ones
  // in GxEPD rotation 0 is used for native orientation (most TFT libraries use 0 fix for portrait orientation)
  // set rotation to 1 (rotate right 90 degrees) to have enough space on small displays (landscape)
  display.setRotation(1);
  // select a suitable font in Adafruit_GFX
  display.setFont(&MY_FONT9);
  // on e-papers black on white is more pleasant to read
  display.setTextColor(GxEPD_BLACK);
  // Adafruit_GFX has a handy method getTextBounds() to determine the boundary box for a text for the actual font
  int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh); // it works for origin 0, 0, fortunately (negative tby!)
  // center bounding box by transposition of origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
  display.setCursor(x, y); // set the postition to start printing text
  display.print(text); // print some text
  //Serial.println("drawHelloWorldForDummies done");
}*/
