#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif

#include <time.h>
#include <ESP32Time.h>
#include "sntp.h"
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define CW1 8
#define WW1 10

#define CW2 3
#define WW2 1

#define CW3 5
#define WW3 6

AsyncWebServer server(80);
ESP32Time rtc(0); //new york utc offset

// Set your Static IP address
IPAddress local_IP(10, 0, 0, 55);
// Set your Gateway IP address
IPAddress gateway(10, 0, 0, 1);

IPAddress subnet(255, 255, 0, 0);

struct State {
  int mode;
  int intensity;
  int temperature;
};

struct State state;

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

void all_set(int duty)
{
   for(int i=0; i<6; i++)
  {
    ledcWrite(i,duty);
  }
}

void warm_set(int duty)
{
   for(int i=3; i<6; i++)
  {
    ledcWrite(i,duty);
  }
}

void cool_set(int duty)
{
   for(int i=0; i<3; i++)
  {
    ledcWrite(i,duty);
  }
}

void setup(void) {

  
  Serial.begin(115200);
  Serial.println("first things first");

  //reset state
  state.mode = 1;
  state.intensity = 512;
  state.temperature = 0;
  
  for(int i=0; i<6; i++)
  {
    if(!ledcSetup(i, 25000,10))
    {
      Serial.println("failed to setup led!");
      while(1)
      {
        //do nothing
      }
    }
  }

  for(int i=0; i<6; i++)
  {
    ledcWrite(i,1023);
  }

  ledcAttachPin(CW1, 0);
  ledcAttachPin(CW2, 1);
  ledcAttachPin(CW3, 2);
  ledcAttachPin(WW1, 3);
  ledcAttachPin(WW2, 4);
  ledcAttachPin(WW3, 5);
  
  initSPIFFS();
  if(!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }

  File wifiCreds = SPIFFS.open("/creds.txt","r");
  DynamicJsonDocument doc(512U);
    if (DeserializationError::Ok == deserializeJson(doc, wifiCreds))
    {
      JsonObject credJson = doc.as<JsonObject>();
      WiFi.mode(WIFI_STA);
      WiFi.setTxPower(WIFI_POWER_19_5dBm);
      WiFi.begin(credJson["ssid"].as<String>().c_str(), credJson["pwd"].as<String>().c_str());
    }
    else{
      Serial.println("failed to retrieve credentials from SPIFFS.");
      while(1){
        //do nothing
      }
    }
  Serial.println("");
  

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected!!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //at this point we should be connected


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi, im a lamp!");
  });

  server.on("/lamp", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/lamp.htm");
  });

    server.on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/scripts.js");
  });

  server.on("/status.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    String jsonStateStr;;
    StaticJsonDocument<512> jsonState;
    jsonState["mode"] = state.mode;
    jsonState["temperature"] = state.temperature;
    jsonState["intensity"] = state.intensity;
    serializeJson(jsonState, jsonStateStr);
    request->send(200, "application/json", jsonStateStr);
  });

  server.on(
    "/lamp",
    HTTP_POST,
     [](AsyncWebServerRequest *request){
    //nothing and dont remove it
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    DynamicJsonDocument jsonDoc(512U);
    if (DeserializationError::Ok == deserializeJson(jsonDoc, (const char*)data))
                {
                    JsonObject obj = jsonDoc.as<JsonObject>();

//                    Serial.printf("%s\n", obj["int"].as<String>().c_str());
                if(obj["cmd"] == "set"){
                warm_set(obj["warm"].as<int>());
                cool_set(obj["cool"].as<int>());
                state.mode = obj["mode"].as<int>();
                state.temperature = obj["temperature"].as<int>();
                state.intensity = obj["intensity"].as<int>();
                }

                request->send(200, "text/plain", "OK");
    } 
    else {
      request->send(404, "text/plain", "");
    }
  });
 
  AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  server.begin();
  Serial.println("HTTP server started");
}

int rtcHour;
int count =0;

void loop(void) {
  
  rtcHour = rtc.getHour(true);
  Serial.print("rtc: ");
  Serial.print(rtcHour);
  if(rtcHour > 8)
  {
    all_set(1024);
    Serial.println(" leds ON!");
  }
  else{
    all_set(0);
    Serial.println(" leds OFF!");
  }
  delay(1000);
  rtc.setTime(count*3600);
  count++;
  
  
}
