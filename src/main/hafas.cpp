#include "hafas.h"
/*
int min(int a, int b){
  return (a < b) ? a : b;
}*/

struct tm parse_time_string(String date_string, String time_string){
  struct tm result;
  int i = 0, day_offset = 0;
  result.tm_year = date_string.substring(0,4).toInt();
  result.tm_mon = date_string.substring(4,6).toInt();
  result.tm_mday = date_string.substring(6).toInt();

  if(time_string.length() > 6){
    i = 2;
    day_offset = time_string.substring(0,2).toInt();
  }

  result.tm_hour = time_string.substring(i, i+2).toInt() + day_offset;
  result.tm_min = time_string.substring(i+2, i+4).toInt();
  result.tm_sec = time_string.substring(i+4).toInt();
  
  return result;
}

String parse_station(String lids){
   
    while(true){
      
      if(lids.charAt(0) == 'O'){
        // Omit the 'O='
        return lids.substring(2,lids.indexOf("@"));
      }
      
      int index = lids.indexOf("@");
      if(index == -1){
        break;       
      }

      lids = lids.substring(index+1);

      if(lids.length() == 0){
        break;
      }

    }

    return "No station";
}

int update_departures(StaticJsonDocument<JSON_BUFFER_SIZE> &buffer){
      WiFiClient client;
      HTTPClient http;
      char date_string[9];
      char time_string[7];
      
      http.begin(client, HAFAS_URL);
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Host", "ESP32866");
      
      sprintf(date_string, "%04i%02i%02i", get_year(), get_month(), get_day());
      sprintf(time_string, "%02i%02i%02i", get_hour(), get_mins(), get_secs());

      String request_body = "{\"lang\": \"de\", \"svcReqL\": [{\"meth\": \"StationBoard\", \"req\": {\"stbLoc\": {\"lid\": \"A=1@L=" + String(HOME_STATION) + "@\"}" 
                            + ", \"date\": \"" + String(date_string) + "\", \"dirLoc\": null, \"time\": \"" + String(time_string) + "\", \"maxJny\": " + String(LEGS_COUNT, DEC) + ", \"type\": \"DEP\"" 
                            + ", \"dur\": -1, \"jnyFltrL\": [{\"mode\": \"INC\", \"value\": \"1023\", \"type\": \"PROD\"}]}}], \"auth\": " 
                            + "{\"aid\": \"r0Ot9FLFNAFxijLW\", \"type\": \"AID\"}, \"client\": {\"id\": \"NAHSH\", \"type\": \"IPH\", \"name\": \"NAHSHPROD\", \"v\": \"3000700\"}, \"ver\": \"1.30\"}";           
    
      int response_code = http.POST(request_body);
      
      if (response_code != HTTP_OK){
        return -1;
      }
      
      DeserializationError err = deserializeJson(buffer, http.getString());
      
      if(err){
        Serial.printf("Error: desieralizeJson() failed: %s\n", err.f_str());
        return -1;
      }
      
      //TODO: kann man bestimmt noch eleganter machen
      if (buffer["err"].as<String>() != "OK" or buffer["svcResL"][0]["err"].as<String>() != "OK"){
        String error;
        if (buffer["err"] != "OK"){
          error = buffer["err"].as<String>();
        }else{
          error = buffer["svcResL"][0]["err"].as<String>() + ": " + buffer["svcResL"][0]["errTxt"].as<String>();
        }
        Serial.printf("Error in Response: %s\n", error);
        return -1;
      }
            
      // Free resources
      http.end();  
      
      return 0;
  }

  
void parse_legs(StaticJsonDocument<JSON_BUFFER_SIZE> &buffer, leg (&legs)[LEGS_COUNT]){
  
  JsonArray legs_array = buffer["svcResL"][0]["res"]["jnyL"].as<JsonArray>();
  int legs_size = legs_array.size();
  
  for(int i = 0; i < min(LEGS_COUNT, legs_size); ++i){
    const char* name_string = buffer["svcResL"][0]["res"]["common"]["prodL"][legs_array[i]["prodX"]]["name"] | "No name";
    const char* direction_string = legs_array[i]["dirTxt"] | "No direction";
    bool has_delay = legs_array[i]["stbStop"].containsKey("dTimeR");
    String date_string = legs_array[i]["date"].as<String>();
    String time_string = legs_array[i]["stbStop"]["dTimeS"].as<String>();
    String delay_string = has_delay ? legs_array[i]["stbStop"]["dTimeR"].as<String>() : time_string;
    
    // Sometimes the API sends a delay, even if there is none
    has_delay = ! time_string.equals(delay_string);
    
    legs[i].scheduled_arrival = parse_time_string(date_string, time_string);
    legs[i].name = String(name_string);
    legs[i].direction = String(direction_string);
    legs[i].station = parse_station(buffer["svcResL"][0]["res"]["common"]["locL"][legs_array[i]["stbStop"]["locX"].as<int>()]["lid"]);
    legs[i].has_delay = has_delay;
    legs[i].delayed_arrival = parse_time_string(date_string,delay_string);
    legs[i].cancelled = legs_array[i]["stbStop"]["dCncl"] | false;   
  }
}

void print_leg(leg &l){
  String arrival = format_date(&(l.scheduled_arrival));
  String delayed_arrival = format_date(&(l.delayed_arrival));
  String cancelled = l.cancelled ? "true" : "false";

  String to_print = String("Leg for ") + l.name + String(":\n") + String("From: ") + l.station + String("\nTo: ") 
  + l.direction + String("\nAt: ") + arrival 
  + (l.has_delay ? String("\nWith delay: ") + delayed_arrival : String("")) 
  + String("\nCancelled: ") + cancelled;
  Serial.println(to_print);
}
