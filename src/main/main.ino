  #include "include/main.h"

StaticJsonDocument<JSON_BUFFER_SIZE> buffer;
leg legs[LEGS_COUNT];

char ssid[SSID_LEN];
char pw[PW_LEN];

void my_panic(){
  for(;;){
    Serial.println("PANIC!");
    delay(1000);
  }
}

String get_saved_ssid(){
  EEPROM.begin(sizeof(credentials));
  credentials c;
  EEPROM.get(0,c);
  return String(c.ssid);
}

String get_saved_pw(){
  EEPROM.begin(sizeof(credentials));
  credentials c;
  EEPROM.get(0,c);
  return String(c.pw);
}

void save_ssid(String ssid){
  EEPROM.begin(sizeof(credentials));
  credentials c;
  strncpy(c.ssid, ssid.c_str(), SSID_LEN);
  strncpy(c.pw, get_saved_pw().c_str(), PW_LEN);
  EEPROM.put(0,c);
  EEPROM.commit();
}

void save_pw(String pw){
  EEPROM.begin(sizeof(credentials));
  credentials c;
  strncpy(c.ssid, get_saved_ssid().c_str(), SSID_LEN);
  strncpy(c.pw, pw.c_str(), PW_LEN);
  EEPROM.put(0,c);
  EEPROM.commit();
}

bool connect_wifi(String ssid, String pw){
  Serial.println("Connecting to: " + ssid + ", with pw: " + pw);
  WiFi.begin(ssid,pw);

  for(int i = 0; i < CONNECTION_RETRIES; ++i){
    if (WiFi.status() == WL_CONNECTED){
      break;
    }

    delay(SECOND);
    Serial.println(String("Connecting... ") + String(i));
  }
    bool connected = WiFi.status() == WL_CONNECTED;
    Serial.println(connected ? "Success!" : "Failure!");
    return connected;
}

bool connect_saved_wifi(){
  return connect_wifi(get_saved_ssid(), get_saved_pw());
}

void change_wifi_pw(){
  print_string("Couldn't Connect to saved Wifi with ssid:\n" + get_saved_ssid() + "\nAnd pw:\n" + get_saved_pw());
  delay(SECOND*3);
  print_string("To reset saved wifi, please create a temporal hotspot with name:\n" + String(TEMP_SSID) + "\nand password:\n" + String(TEMP_PW));

  for(;;){
      if (connect_wifi(String(TEMP_SSID), String(TEMP_PW))){
        break;
      }
  }

  ESP8266WebServer server(80);
  String ssid = get_saved_ssid();
  String pw = get_saved_pw();
  bool credentials_set = false;

  server.on("/setup/", [&server, &ssid, &pw, &credentials_set] (){
    Serial.println("Server [/setup/]: ");
    if (server.method() != HTTP_POST) {
      server.send(405, "text/plain", "Method Not Allowed. Please use POST.");
    } else {
      String body = server.arg("plain");
      Serial.println("Parsed body: " + body);
      DeserializationError err = deserializeJson(buffer, body);

      if(err || !buffer.containsKey("ssid") || !buffer.containsKey("password")){
        Serial.printf("Error: desieralizeJson() failed: %s\n", err.f_str());
        server.send(400, "text/plain", "Post body malformed. Body: '" + body + "'");
      } else {
        ssid = buffer["ssid"].as<String>();
        pw = buffer["password"].as<String>();
        credentials_set = true;
        server.send(200, "text/plain", "Success! Saving new Wifi ssid: " + ssid + "\nand password: " + pw);
      }
    }
  });

  server.on("/", [&server, &ssid, &pw]() {
    server.send(200, "text/plain", String("Hello from ESP8266!\n")
    + String("To set the wifi login data, run the following command in a command shell, inserting your ssid and password.\nOn Windows, you con use cmd.exe, on Linux, use a Terminal.")
    + String("\n\n[WINDOWS]: curl http://" + WiFi.localIP().toString() + "/setup/ -X POST -d \"{\\\"ssid\\\": \\\"enter your ssid here\\\", \\\"password\\\" : \\\"enter your password here\\\"}\"")
    + String("\n\n[LINUX]: curl http://" + WiFi.localIP().toString() + "/setup/ -X POST -d '{\"ssid\": \"enter your ssid here\", \"password\" : \"enter your password here\"}'")
    + String("\n\nSSID currently set to: '" + ssid + String("'\nPW currently set to: '") + pw + String("'")));
  });

  print_string(
    String("Join your hotspot with a laptop, and open following url in a browser:\n") +
    String("http://" + WiFi.localIP().toString() + "/")
  );

  server.begin();
  while (!credentials_set){
    Serial.println("Loop credentials_set: " + String(credentials_set));
    server.handleClient();
  }

  save_ssid(ssid);
  save_pw(pw);

  Serial.println("Saved ssid: " + get_saved_ssid() + "\npw: " + get_saved_pw());
}

void init_wifi(){
  Serial.println("Connecting to wifi...");
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  //TODO: remove true
  while (true || !connect_saved_wifi()){
    change_wifi_pw();
  }
}

void setup()
{
  Serial.setDebugOutput(true);
  Serial.begin(9600);
  
  init_display();
  init_wifi();
  init_time();
}

void loop() {
  delay(3000);
  
  Serial.printf("\nIt is the %02i:%02i:%04i at %02i:%02i:%02i o' clock.\n", get_day(), get_month(), get_year(), get_hour(),get_mins(),get_secs());
  
  if(WiFi.status() == WL_CONNECTED){
      if(update_departures(buffer) != 0){
        Serial.println("No departures!");
        return;
      }

     parse_legs(buffer, legs);

     Serial.println("\n\n\nParsed legs\n\n:");
     for( int i = 0; i < LEGS_COUNT; ++i){
      print_leg(legs[i]);
      Serial.println();
     }

     erase_display();
     upper(legs[0]);
     lower(legs[1]);
     update_display();
    }
    else {
      Serial.println("Lost Wifi Connection!");
    }
}
