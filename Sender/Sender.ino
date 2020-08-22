//#line 1 "C:\\Users\\User\\Arduino\\sketch_jul13b\\sketch_jul13b.ino"

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
SoftwareSerial NodeMCU(D2,D3);
enum task {
  DROP_FOOD = 1,
  MAKE_NOISE,
  DISTANCE
};

//-------------------Functions Delerations---------
void initializeWifi(char  ssid [], char  password []);
void handleDropFood();
void initializeSerial();
void handleMakeNoise();
void notrifyServerMakeNoise();
void sendIdToServer();
void notrifyServerFoodDroped();
void handleDistance();
void notrifyServerDistance();
void checkNotification();
void sendNoFoodNotification();
void setup() {
  initializeSerial();
  initializeWifi("DEKEL_TSAIRI", "18181818");
  sendIdToServer();
}

void loop() {
//  NodeMCU.print(recivedData);
//  delay(1000);
//  recivedData = NodeMCU.parseInt();
//  Serial.print(recivedData);




  
  //Serial.print("in main loop");
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    Serial.print("[HTTP] begin...\n");
    String url = String("http://petfeeder-app.herokuapp.com/api/v1.0/arduinos/");
    url.concat(ESP.getChipId());
    url.concat("/flags");
    if (http.begin(client, url)) { 
      
      Serial.print("[HTTP] FETCHING FLAGS...\n");
      int httpCode = http.GET();
      
      if (httpCode > 0) {
        
        Serial.printf("[HTTP] GET FLAGS... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          
          String json = http.getString();

          StaticJsonDocument<200> doc;
          DeserializationError error = deserializeJson(doc, json);
          
          if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.c_str());
              return;
          }
          else{
            const bool isDropFood= doc["dropFood"];

            if (isDropFood){
              Serial.println("handle frop food!");
              handleDropFood();
            }

            const bool isMakeNoise= doc["makeNoise"];

            if (isMakeNoise){
              Serial.println("handle make noise!");
              handleMakeNoise();
            }

            const bool isDistance = doc["distance"];
            
            if (isDistance ){
              Serial.println("handle distance!");
              handleDistance();
            }
            checkNotification();
          }
        }
        else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
      http.end();
    } 
    else {
        Serial.printf("[HTTP} Unable to connect\n");
    }
  }
 }
  delay(10000);
}

void checkNotification(){
  if (NodeMCU.available() > 0){
    int distance = NodeMCU.parseInt();
    Serial.print("distance is: ");
    Serial.println(distance);

    if (distance  > 50){
      Serial.println("going to notify server there is no food");
      sendNoFoodNotification();
    }
  }; // busy waiting 
  
}

void sendNoFoodNotification(){
    WiFiClient client1;
    HTTPClient http1;
    String url = String("http://petfeeder-app.herokuapp.com/api/v1.0/arduinos/");
    url.concat(ESP.getChipId());
    url.concat("/flags");
    Serial.print("[HTTP] begin...\n");
    if (http1.begin(client, url)) {  // HTTP
      Serial.print("[HTTP] POST No Food notification ...\n");
      String target = String("{\"isContainerEmpty\":\"");
      target.concat(true);
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
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
//  String url = String("http://localhost:3000/api/v1.0/arduinos/");
//  url.concat(ESP.getChipId());
//  url.concat("/flags");
//
//  DynamicJsonDocument doc(2048);
//  doc["isContainerEmpty"] = true;
//  String json;
//  serializeJson(doc, json);
//  WiFiClient client1;
//  HTTPClient http1;
//  if (http1.begin(client1, url)) { 
//    http1.addHeader("Content-Type", "application/json");
//    Serial.println("sendNoFoodNitfication: in if statement");
//    http1.POST(json);
//  }
//  Serial.print(http1.getString());
//  http1.end();
}
void initializeSerial(){
  Serial.begin(9600);
  NodeMCU.begin(4800);
  pinMode(D2,INPUT);
  pinMode(D3,OUTPUT);
  
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }  
}


void initializeWifi(char ssid [], char password []){
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  //Serial.println("finishe initialize wifi");
}
void handleDropFood(){
  NodeMCU.print(DROP_FOOD);
  delay(1000);
  notrifyServerFoodDroped();   
}

void handleDistance(){
  NodeMCU.print(DISTANCE);
  delay(1000);
  while (NodeMCU.available() < 0){}; // busy waiting 
  int distance = NodeMCU.parseInt();
  Serial.print("distance is: ");
  Serial.println(distance);

  notrifyServerDistance(); 
}


void handleMakeNoise(){
  NodeMCU.print(MAKE_NOISE);
  delay(1000);
  notrifyServerMakeNoise();
}

void notrifyServerDistance(){
  String url = String("http://petfeeder-app.herokuapp.com/api/v1.0/arduinos/");
  url.concat(ESP.getChipId());
  url.concat("/distance");


  

  
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
