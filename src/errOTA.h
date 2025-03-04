
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WebServer.h>
#elif defined(TARGET_RP2040)
  #include <WiFi.h>
  #include <WebServer.h>
#endif

#include <ElegantOTA.h>

const char* ssid = "Turangga AP";
const char* password = "TuranggaAP123";

#if defined(ESP8266)
  ESP8266WebServer server(80);
#elif defined(ESP32)
  WebServer server(80);
#elif defined(TARGET_RP2040)
  WebServer server(80);
#endif

unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

String devicename = "Led Matrix WL";
int ipne = 13;

IPAddress local_IP(192, 168, 43, ipne);  // IP Static
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);

    

void setupOTA() {
    WiFi.mode(WIFI_STA);
    
    WiFi.config(local_IP, gateway, subnet);
    WiFi.begin(ssid, password);
    Serial.println("");
  
    // Wait for connection
    // while (WiFi.status() != WL_CONNECTED) {
    //   delay(500);
    //   Serial.print(".");
    // }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  
    // server.on("/", []() {
    //   server.send(200, "text/plain", "FLUX V1.0 | " + devicename);


      
    // });


    server.on("/", []() {
      server.send(200, "text/html", R"rawliteral(
          <html>
          <head>
              <title>NOCOLA</title>
              <style>
                  body { font-family: Arial; text-align: center; background: #f4f4f4; }
                  h1 { color: #333; }
                  button { padding: 10px; font-size: 16px; }
              </style>
          </head>
          <body>
              <h1>)rawliteral" + devicename + R"rawliteral(</h1>
              <button onclick="window.location.href='/update'">Update Now</button>
          </body>
          </html>
      )rawliteral");
  });

    ElegantOTA.begin(&server);    // Start ElegantOTA
    // ElegantOTA callbacks
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);
  
    server.begin();
    Serial.println("HTTP server started");
  
}