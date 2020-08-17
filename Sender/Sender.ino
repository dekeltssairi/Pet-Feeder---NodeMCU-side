#line 1 "C:\\Users\\User\\Arduino\\sketch_jul13b\\sketch_jul13b.ino"

//--------------Libreries--------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h> 
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include<SoftwareSerial.h> 

//-------------------Globals-----------------------
ESP8266WiFiMulti WiFiMulti;
WiFiClient client;
HTTPClient http;
SoftwareSerial s(3,1);

//-------------------Functions Delerations---------
void initializeWifi(char  ssid [], char  password []);
void handleDropFood();
void initializeSerial();
void handleMakeNoise();
void notrifyServerMakeNoise();
void sendIdToServer();
void notrifyServerFoodDroped();

void setup() {
  initializeSerial();
  initializeWifi("DEKEL_TSAIRI", "18181818");
  sendIdToServer();
  s.begin(9600);
}

void loop() {
  //Serial.print("in main loop");
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    //Serial.print("[HTTP] begin...\n");
    String url = String("http://petfeeder-app.herokuapp.com/api/v1.0/arduinos/");
    url.concat(ESP.getChipId());
    url.concat("/flags");
    if (http.begin(client, url)) { 
      
      //Serial.print("[HTTP] FETCHING FLAGS...\n");
      int httpCode = http.GET();
      
      if (httpCode > 0) {
        
        //Serial.printf("[HTTP] GET FLAGS... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          
          String json = http.getString();

          StaticJsonDocument<200> doc;
          DeserializationError error = deserializeJson(doc, json);
          
          if (error) {
              //Serial.print(F("deserializeJson() failed: "));
              //Serial.println(error.c_str());
              return;
          }
          else{
            const bool isDropFood= doc["dropFood"];

            if (isDropFood){
              handleDropFood();
            }

            const bool isMakeNoise= doc["makeNoise"];

            if (isMakeNoise){
              handleMakeNoise();
            }
          }
        }
        else {
//          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
      http.end();
    } 
    else {
//      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
 }
  delay(10000);
}

void initializeSerial(){
  Serial.begin(9600);
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }  
}


void initializeWifi(char ssid [], char password []){
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  Serial.println("finishe initialize wifi");
}
void handleDropFood(){
   
  s.write(1);
  delay(1000);
  notrifyServerFoodDroped();   
}


void handleMakeNoise(){
  //Serial.println("making Noise!");
  s.write(2);
  delay(1000);
  notrifyServerMakeNoise();
}

void notrifyServerMakeNoise(){
  String url = String("http://petfeeder-app.herokuapp.com/api/v1.0/arduinos/");
  url.concat(ESP.getChipId());
  url.concat("/makeNoise");
  if (http.begin(client, url)) { 
//    Serial.println("In if statement");
    http.POST("{\"made\":\"noise!\"}");
  }
}


void notrifyServerFoodDroped(){
  String url = String("http://petfeeder-app.herokuapp.com/api/v1.0/arduinos/");
  url.concat(ESP.getChipId());
  url.concat("/dropFood");
  if (http.begin(client, url)) { 
//    Serial.println("In if statement");
    http.POST("{\"food\":\"dropded!\"}");
  }
}


void sendIdToServer(){
  Serial.println("Sending id to Server");
  bool isIdSent = false;
  while(WiFiMulti.run() == WL_CONNECTED && !isIdSent)
  {
    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://petfeeder-app.herokuapp.com/api/v1.0/arduinos/new")) {  // HTTP

      ESP.getChipId();
      Serial.print("[HTTP] POST Arduino Id...\n");
      String target = String("{\"arduinoId\":\"");
      target.concat(ESP.getChipId());
      target.concat("\"}");
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(target);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
         Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_CREATED || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          isIdSent = true;
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
}
