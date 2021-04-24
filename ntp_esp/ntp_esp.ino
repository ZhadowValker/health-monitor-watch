
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid          = "wifi 2.4";      // The SSID (name) of your Wi-Fi network
const char* password  = "m4n1f3s7";      // The password of the Wi-Fi network
const char* host      = "http://api.timezonedb.com/v2/get-time-zone?key=8O759IYE94G9&format=xml&fields=formatted&by=zone&zone=Asia/Kolkata";

String payload;               // Variables to accept data
String nowday;
String nowmonth;
String nowyear;
String nowhour;
String nowmin;
String nowsec;

unsigned long prevMillis = millis();
unsigned long interval = 30000;                 // Refresh in loop every 30 seconds


void setup() {
  Serial.begin(115200);                     // Start serial communication
  delay(10);
  Serial.println('\n');

  wifi();
  tzdb();
  parse_response();
}

void wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();                                       // Clear any existing connection
  WiFi.begin(ssid, password);                         // Access WiFi
   
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print(" ...");

  while (WiFi.status() != WL_CONNECTED) {   // Wait for WiFi to connect
    delay(1000);
    Serial.print(".");
  }

  Serial.println('\n');
  Serial.println("WiFi connection established");
  Serial.print("Device's IP address is ");
  Serial.println(WiFi.localIP());                        // Show device's IP address
}

void tzdb() {
  int httpCode = 0;                                        // Variable to hold received data
  HTTPClient http;                                          // Declare an object of class HTTPClient
 
  Serial.println("Connecting to TimezoneDB...");

  http.begin(host);                                        // Connect to site
  httpCode = http.GET();                               // Check if data is coming in

  while (httpCode == 0) {                             // if no data is in
    delay(1000);                                           // wait a sec
    http.begin(host);                                     // and try again
    httpCode = http.GET();
  }

  payload = http.getString();                        // Save response as string
  Serial.println(payload);                              // Show response
 
  //Sample response:
  //<?xml version="1.0" encoding="UTF-8"?>
  //<result><status>OK</status><message/><formatted>2018-02-08 14:24:16</formatted></result>

//  http.end();                                                // Close connection to timezonedb
//  WiFi.mode(WIFI_OFF);                               // Close connection to WiFi
}

void parse_response() {
  int colon = payload.indexOf(':');                 // Set the first colon in time as reference point
  int d;                                                        // Variables to hold data as integers
  int mo;
  int y;
  int h;
  int mi;
  int s;

  nowday = payload.substring(colon - 5, colon - 3);     // Get data as substring
  d = nowday.toInt();                                                // and convert to int
  nowmonth = payload.substring(colon - 8, colon - 6);
  mo = nowmonth.toInt();
  nowyear = payload.substring(colon - 13, colon - 9);
  y = nowyear.toInt();

  nowhour = payload.substring(colon - 2, colon);
  h = nowhour.toInt();
  nowmin = payload.substring(colon + 1, colon + 3);
  mi = nowmin.toInt();
  nowsec = payload.substring(colon + 4, colon + 6);
  s = nowsec.toInt();

  // Print integers without leading zeros - use in calculations

  Serial.print("Today's date is:   ");
  Serial.print(y);
  Serial.print("-");
  Serial.print(mo);
  Serial.print("-");
  Serial.println(d);

  Serial.print("Current local time is:   ");
  Serial.print(h);
  Serial.print(":");
  Serial.print(mi);
  Serial.print(":");
  Serial.println(s);
  Serial.println();

  /*Print characters with leading zeros - use for lcd etc.

    Serial.print("Today's date is:   ");
    Serial.print(nowyear);
    Serial.print("-");
    Serial.print(nowmonth);
    Serial.print("-");
    Serial.println(nowday);

    Serial.print("Current local time is:   ");
    Serial.print(nowhour);
    Serial.print(":");
    Serial.print(nowmin);
    Serial.print(":");
    Serial.println(nowsec);
    Serial.println();
  */
}

void loop() {

  if (millis() - prevMillis > interval) {                              // refresh
    wifi();
    tzdb();
    parse_response();
    prevMillis = millis();
  }
}
