#pragma once
#define WIFI_STA 1
#define WIFI_AP_STA 3
#define WL_CONNECTED 1
#define WIFI_POWER_8_5dBm 34
struct IPAddress {
 String s;
 bool fromString(const String&v){int a,b,c,d;char e;if(sscanf(v.c_str(),"%d.%d.%d.%d%c",&a,&b,&c,&d,&e)!=4)return false;for(int x:{a,b,c,d})if(x<0||x>255)return false;s=v;return true;}
 String toString()const{return s;}
};
struct WiFiClass {
 int connection=0,apStarts=0,apStops=0,configs=0,txPower=0;String ssid,password,hostname,apPassword;
 void mode(int){} void setHostname(const char*h){hostname=h;} void setTxPower(int p){txPower=p;}
 void config(IPAddress,IPAddress,IPAddress,IPAddress){configs++;}
 void softAP(const char*,const char*p){apStarts++;apPassword=p;} void begin(const char*s,const char*p){ssid=s;password=p;}
 int status(){return connection;} void softAPdisconnect(bool){apStops++;} IPAddress localIP(){IPAddress i;i.s="192.0.2.1";return i;}
 int RSSI(){return -60;}
};
inline WiFiClass WiFi;
