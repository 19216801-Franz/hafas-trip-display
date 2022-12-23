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

void reset_wifi(){
  print_string("Couldn't connect\nto wifi\n" + get_saved_ssid() + "\nwith pw:\n" + get_saved_pw());
  delay(SECOND*3);
  print_string("To reset saved wifi,\nplease create a hotspot with name:\n" + String(TEMP_SSID) + "\nand password:\n" + String(TEMP_PW));

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
    + String("To set the wifi login data, run the following command in a command shell, inserting your ssid and password.\nOn Windows, you can use cmd.exe.")
    + String("\n\n[WINDOWS]: curl http://" + WiFi.localIP().toString() + "/setup/ -X POST -d \"{\\\"ssid\\\": \\\"enter your ssid here\\\", \\\"password\\\" : \\\"enter your password here\\\"}\"")
    + String("\n\n[LINUX]: curl http://" + WiFi.localIP().toString() + "/setup/ -X POST -d '{\"ssid\": \"enter your ssid here\", \"password\" : \"enter your password here\"}'")
    + String("\n\nSSID currently set to: '" + ssid + String("'\nPW currently set to: '") + pw + String("'")));
  });

  print_string(
    //String malformed to fit screen perfectly
    String("Join your hotspot witha laptop, and open\nfollowing url in\na browser:\n") +
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

  while (!connect_saved_wifi()){
    reset_wifi();
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
  /*TODO: Final test: Set wrong saved password,
  restart, set correct password and let the microcontroller restart a couple times
  save_ssid("WRONG SSID");
  save_pw("WRONG PW");
  */

  if(WiFi.status() != WL_CONNECTED){
    //TODO: handle connection loss
    Serial.println("Lost Wifi Connection! Initialising wifi again.");
    init_wifi();
  }

  
  if(update_departures(buffer) != 0){
    Serial.println("Couldn't update departures!");
    power_display_up();
    update_info_window("[Error] Couldn't update departures!");
    power_display_down();
    return;
  }

  int need_refresh = parse_legs(buffer, legs);
  Serial.printf("Do I need a refresh? %i\n", need_refresh);

  Serial.println("\nParsed legs:");
  for( int i = 0; i < LEGS_COUNT; ++i){
    print_leg(legs[i]);
    Serial.println();
  }

  if (need_refresh){
    power_display_up();
    erase_display();
    print_leg_to_display(FIRST_WINDOW_START, legs[0]);
    print_leg_to_display(SECOND_WINDOW_START, legs[1]);
    update_info_window();
    power_display_down();
  }

  delay(SECOND * 60);
}
