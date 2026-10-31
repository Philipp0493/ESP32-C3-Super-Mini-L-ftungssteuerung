#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <esp_system.h>
#include "ui.h"

// Manufacturer RS485 installation guide, June 2025. Single master only.
constexpr int RX_PIN=4, TX_PIN=5;
HardwareSerial bus(1);
WebServer web(80);
Preferences prefs;
String mode="off", phase="disabled", apiKey;
bool offSent=false;
bool enabled=false, waiting=false, receiving=false, pauseSent=false;
uint8_t level=1, humidity=2;
bool night=false, supply=true;
uint32_t lastSend=0, phaseStart=0, boostStart=0, receiveStart=0, lastReply=0;
String frame, reply;
String modeBeforeBoost="wrg";
uint8_t previousLevel=1, previousHumidity=2;
bool previousNight=false, inFrame=false, hasReply=false;
bool apActive=false;
uint32_t apStarted=0, disconnectedSince=0;
bool apWindowExpired=false;
constexpr const char *SETUP_PASSWORD="Lueftung-Start";
void startAP(){
 String name="Ambientika-"+String((uint32_t)ESP.getEfuseMac(),HEX);
 WiFi.softAP(name.c_str(),SETUP_PASSWORD);apActive=true;apStarted=millis();
}
int light=-1, filter1=-1, filter2=-1, moist=-1;

bool elapsed(uint32_t now,uint32_t since,uint32_t duration){return uint32_t(now-since)>=duration;}
void sendPayload(const char *payload){bus.write(2);bus.print(payload);bus.write(3);bus.flush();}
void pauseMotor(){phase="pause";waiting=true;pauseSent=false;}
void setMode(String next){mode=next;offSent=false; supply=true; pauseMotor();if(next=="off"||next=="pause")waiting=false;}
String status(){
 return String("{\"enabled\":")+(enabled?"true":"false")+",\"mode\":\""+mode+"\",\"phase\":\""+phase+"\",\"level\":"+level+",\"night\":"+(night?"true":"false")+",\"humidity_level\":"+humidity+",\"device_status_received\":"+(hasReply?"true":"false")+",\"status_age_ms\":"+(hasReply?String(uint32_t(millis()-lastReply)):"null")+",\"dark\":"+light+",\"filter_1\":"+filter1+",\"filter_2\":"+filter2+",\"humidity_alarm\":"+moist+"}";
}
bool auth(){if(!apiKey.isEmpty() && (web.arg("key")==apiKey || web.header("X-API-Key")==apiKey))return true;web.send(401,"text/plain","API key required");return false;}
bool validMode(String s){return s=="off"||s=="pause"||s=="wrg"||s=="auto"||s=="supply"||s=="extract"||s=="boost";}
void control(){
 if(!auth())return;
 String next=web.arg("mode");
 if(!validMode(next)){web.send(400,"text/plain","mode: off,pause,wrg,auto,supply,extract,boost");return;}
 String l=web.arg("level"), h=web.arg("humidity"), n=web.arg("night");
 if((l.length()&&l!="0"&&l!="1"&&l!="2"&&l!="3")||(h.length()&&h!="2"&&h!="3")||(n.length()&&n!="0"&&n!="1")){web.send(400,"text/plain","Invalid level/humidity/night");return;}
 if(!enabled){web.send(409,"text/plain","Enable bus after checking wiring first");return;}
 if(next=="boost"&&mode=="boost"){web.send(200,"application/json",status());return;}
 if(next=="boost"){previousLevel=level;previousHumidity=humidity;previousNight=night;}
 bool changed=next!=mode || (l.length()&&level!=l.toInt()) || (h.length()&&humidity!=h.toInt()) || (n.length()&&night!=(n=="1"));
 if(l.length())level=l.toInt();if(h.length())humidity=h.toInt();if(n.length())night=n=="1";
 // Repeated Loxone commands must not restart the cycle or extend a boost.
 if(changed){if(next=="boost"&&mode!="boost"){modeBeforeBoost=(mode=="off"||mode=="pause")?"wrg":mode;}setMode(next);}
 web.send(200,"application/json",status());
}
void root(){
 String html=UI_PAGE;html.replace("{{KEY}}",apiKey);web.send(200,"text/html; charset=utf-8",html);
}
void setup(){
 Serial.begin(115200);
 uint32_t usbWait=millis();while(!Serial && !elapsed(millis(),usbWait,3000))delay(10);
 bus.begin(9600,SERIAL_8N1,RX_PIN,TX_PIN);
 prefs.begin("ambientika",false);apiKey=prefs.getString("key","");
 if(apiKey.isEmpty()){char key[33];for(int i=0;i<4;i++)snprintf(key+8*i,9,"%08lx",(unsigned long)esp_random());apiKey=key;prefs.putString("key",apiKey);}
 enabled=prefs.getBool("enabled",false); // Boot always selects off, never resume a boost.
 WiFi.mode(WIFI_AP_STA);String suffix=String((uint32_t)ESP.getEfuseMac(),HEX);String ap="Ambientika-"+suffix;
 String apPassword=SETUP_PASSWORD;startAP();
 Serial.println("Setup AP: "+ap);Serial.println("Setup AP password: "+apPassword);Serial.println("Setup page: 192.168.4.1");
 String ssid=prefs.getString("ssid","");if(ssid.length())WiFi.begin(ssid.c_str(),prefs.getString("pass","").c_str());
 const char* headers[]={"X-API-Key"};web.collectHeaders(headers,1);
 web.on("/network",HTTP_GET,[]{web.send(200,"application/json",String("{\"connected\":")+(WiFi.status()==WL_CONNECTED?"true":"false")+",\"ip\":\""+WiFi.localIP().toString()+"\"}");});web.on("/",HTTP_GET,root);web.on("/api/status",HTTP_GET,[]{web.send(200,"application/json",status());});
 web.on("/api/control",HTTP_GET,control);web.on("/api/control",HTTP_POST,control);
 web.on("/enable",HTTP_POST,[]{if(!auth())return;if(web.arg("enabled")!="0"&&web.arg("enabled")!="1"){web.send(400);return;}if(web.arg("enabled")=="0"&&(mode!="off"||!offSent)){web.send(409,"text/plain","Bitte zuerst Aus wählen und mindestens eine Sekunde warten.");return;}enabled=web.arg("enabled")=="1";prefs.putBool("enabled",enabled);setMode("off");web.send(200,"text/plain","OK");});
 web.on("/api/query",HTTP_POST,[]{if(!auth())return;if(!enabled||receiving){web.send(409);return;}while(bus.available())bus.read();sendPayload("0202");receiving=true;inFrame=false;receiveStart=millis();frame="";web.send(202,"text/plain","Query sent; no command transmission for 3 seconds");});
 web.on("/wifi",HTTP_POST,[]{if(!auth())return;String s=web.arg("ssid"),p=web.arg("password");if(s.isEmpty()||s.length()>32||p.length()>63||(p.length()>0&&p.length()<8)){web.send(400);return;}prefs.putString("ssid",s);prefs.putString("pass",p);apStarted=millis();WiFi.begin(s.c_str(),p.c_str());web.send(200,"text/plain","WLAN gespeichert; Verbindung wird aufgebaut");});
 web.begin();
}
void loop(){
 web.handleClient();uint32_t now=millis();
 if(Serial.available() && Serial.read()=='i'){Serial.println("Setup AP: Ambientika-"+String((uint32_t)ESP.getEfuseMac(),HEX));Serial.println("AP password: "+String(SETUP_PASSWORD));Serial.println("API key: "+apiKey);Serial.println(WiFi.localIP());}
 if(WiFi.status()==WL_CONNECTED){
  disconnectedSince=0;
  // Keep setup AP briefly so the setup page can show the new address.
  if(apActive && elapsed(now,apStarted,60000)){WiFi.softAPdisconnect(true);apActive=false;}
 }else{
  if(!disconnectedSince)disconnectedSince=now;
  if(!apActive && !apWindowExpired && elapsed(now,disconnectedSince,60000))startAP();
 }
 // Setup access is limited to ten minutes; power cycle to reopen it.
 if(apActive&&elapsed(now,apStarted,600000)){WiFi.softAPdisconnect(true);apActive=false;apWindowExpired=true;disconnectedSince=now;}
 while(bus.available()){
  char c=bus.read();if(c==2){frame="";inFrame=true;}else if(c==3){
   // Guide says 11 bytes but its literal frame contains 12; parse literal 00 + 8 registers.
   if(receiving&&inFrame&&!elapsed(now,receiveStart,3000)&&frame.length()==10&&frame.startsWith("00")){bool ok=true;for(int i=2;i<10;i++)if(frame[i]!='0'&&frame[i]!='1')ok=false;if(ok){reply=frame;light=frame[2]-'0';filter1=frame[3]-'0';filter2=frame[4]-'0';moist=frame[5]-'0';lastReply=now;hasReply=true;}}
   frame="";inFrame=false;
  }else if(inFrame){if(frame.length()<32)frame+=c;else{inFrame=false;frame="";}}
 }
 if(!enabled){phase="disabled";return;}
 if(receiving){if(!elapsed(now,receiveStart,3000))return;receiving=false;}
 if(mode=="boost"&&!waiting&&elapsed(now,boostStart,1200000)){level=previousLevel;humidity=previousHumidity;night=previousNight;setMode(modeBeforeBoost);}
 // Start the pause clock when the first pause frame actually reaches the bus.
 if(waiting&&!pauseSent){sendPayload("01200021");phaseStart=millis();lastSend=phaseStart;pauseSent=true;return;}
 if(waiting&&elapsed(now,phaseStart,10000)){waiting=false;phaseStart=now;if(mode=="boost")boostStart=now;phase=supply?"supply":"extract";}
 if(!waiting&&(mode=="wrg"||mode=="auto")&&elapsed(now,phaseStart,60000)){supply=!supply;pauseMotor();}
 if(!elapsed(now,lastSend,500))return;lastSend=now;
 if(mode=="off"){phase="off";sendPayload("01000001");offSent=true;return;}
 if(waiting||mode=="pause"){phase="pause";sendPayload("01200021");return;}
 uint8_t command=0;
 if(mode=="auto")command=(humidity==3?0xA0:0x60)+(supply?(night?8:10):(night?4:6));
 else {bool in=mode=="supply"||((mode=="wrg")&&supply);command=0x20+(in?8:4)+(mode=="boost"?3:level);phase=in?"supply":"extract";}
 char payload[9];snprintf(payload,sizeof(payload),"01%02X00%02X",command,command^1);sendPayload(payload);
}
