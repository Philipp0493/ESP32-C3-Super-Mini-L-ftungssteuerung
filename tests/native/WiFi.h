#pragma once
#define WIFI_AP_STA 0
#define WL_CONNECTED 1
struct WiFiClass {
 int connection=0,apStarts=0,apStops=0;String ssid,password;
 void mode(int){} void softAP(const char*,const char*){apStarts++;} void begin(const char*s,const char*p){ssid=s;password=p;}
 int status(){return connection;} void softAPdisconnect(bool){apStops++;} String localIP(){return "192.0.2.1";}
};
inline WiFiClass WiFi;
