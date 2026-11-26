#pragma once
#define WIFI_STA 1
#define WIFI_AP_STA 3
#define WL_CONNECTED 1
#define WIFI_POWER_8_5dBm 34
struct IPAddress {
 String s;
 IPAddress(){}
 explicit IPAddress(uint32_t a){char b[16];snprintf(b,16,"%u.%u.%u.%u",a&255,(a>>8)&255,(a>>16)&255,a>>24);s=b;}
 bool fromString(const String&v){int a,b,c,d;char e;if(sscanf(v.c_str(),"%d.%d.%d.%d%c",&a,&b,&c,&d,&e)!=4)return false;for(int x:{a,b,c,d})if(x<0||x>255)return false;s=v;return true;}
 String toString()const{return s;}
};
typedef enum {ARDUINO_EVENT_WIFI_STA_GOT_IP,ARDUINO_EVENT_WIFI_STA_DISCONNECTED} arduino_event_id_t;
struct arduino_event_info_t {struct {uint8_t reason;} wifi_sta_disconnected;};
typedef void (*WiFiEventCb)(arduino_event_id_t,arduino_event_info_t);
struct WiFiClass {
 WiFiEventCb eventCb=nullptr; void onEvent(WiFiEventCb cb){eventCb=cb;}
 String SSID(){return ssid;} int channel(){return 6;} IPAddress gatewayIP(){IPAddress i;i.s="192.0.2.254";return i;}
 int connection=0,apStarts=0,apStops=0,configs=0,txPower=0;String ssid,password,hostname,apPassword;
 void mode(int){} void setHostname(const char*h){hostname=h;} void setTxPower(int p){txPower=p;}
 void config(IPAddress,IPAddress,IPAddress,IPAddress){configs++;}
 void softAP(const char*,const char*p){apStarts++;apPassword=p;} void begin(const char*s,const char*p){ssid=s;password=p;begins++;}
 int status(){return connection;} void softAPdisconnect(bool){apStops++;} IPAddress localIP(){IPAddress i;i.s="192.0.2.1";return i;}
 int RSSI(){return -60;}
 bool autoReconnect=true;int begins=0; void persistent(bool){} void setAutoReconnect(bool v){autoReconnect=v;} IPAddress softAPIP(){IPAddress i;i.s="192.168.4.1";return i;}
};
inline WiFiClass WiFi;
