#include <cassert>
#include <iostream>
#include "../../src/main.cpp"
void tick(uint32_t n){fakeNow=n;loop();}
void expect(const char*body){assert(bus.out==std::string(1,2)+body+std::string(1,3));bus.out.clear();}
void request(const char*m){web.args={{"mode",m},{"key",apiKey}};control();}
int main(){
 setup();apiKey="test-key";
 tick(1000);assert(bus.out.empty()); // disabled sends nothing
 request("wrg");assert(web.code==409);
 enabled=true;web.args={{"mode","wrg"},{"key","wrong"}};control();assert(web.code==401);
 web.args={{"mode","wrg"},{"key",apiKey},{"level","4"}};control();assert(web.code==400);
 request("wrg");assert(web.code==200);tick(1100);expect("01200021");
 tick(11099);expect("01200021");assert(waiting);
 tick(11100);assert(bus.out.empty());assert(!waiting);
 tick(11600);expect("01290028"); // supply level 1
 auto start=phaseStart;request("wrg");assert(phaseStart==start&&!waiting);
 tick(71100);expect("01200021"); // first direction change must pause
 tick(71101);expect("01200021");assert(waiting);
 tick(81100);expect("01200021");assert(waiting);
 tick(81601);expect("01250024");assert(!waiting); // extract only after full pause
 request("boost");tick(82000);expect("01200021");
 tick(92000);expect("01270026");auto boostAt=boostStart;
 tick(92500);expect("01270026");request("boost");assert(boostAt==boostStart&&!waiting);
 tick(boostAt+1199999);expect("01270026");assert(mode=="boost");
 tick(boostAt+1200000);expect("01200021");assert(mode=="wrg"&&waiting);
 // Query must suppress all control transmissions for 3 seconds.
 web.args={{"key",apiKey}};web.routes["/api/query"]();expect("0202");auto queried=fakeNow;
 bus.in=std::string(1,2)+"0010110000"+std::string(1,3);tick(queried+100);assert(bus.out.empty());assert(light==1&&filter1==0&&filter2==1&&moist==1);
 tick(queried+2999);assert(bus.out.empty());tick(queried+3000);expect("01200021");
 // Explicit stop never waits for a running phase to finish.
 request("off");tick(fakeNow+500);expect("01000001");
 // Exercise actual encoder for every documented manual/auto operating frame.
 for(int direction=0;direction<2;direction++)for(int speed=0;speed<4;speed++){
  mode=direction?"supply":"extract";level=speed;waiting=false;
  tick(fakeNow+500);char body[9];int c=0x20+(direction?8:4)+speed;
  snprintf(body,9,"01%02X00%02X",c,c^1);expect(body);
 }
 for(int h=2;h<=3;h++)for(int n=0;n<2;n++)for(int d=0;d<2;d++){
  mode="auto";humidity=h;night=n;supply=d;phaseStart=fakeNow;waiting=false;
  tick(fakeNow+500);char body[9];int c=(h==3?0xa0:0x60)+(d?(n?8:10):(n?4:6));
  snprintf(body,9,"01%02X00%02X",c,c^1);expect(body);
 }
 // Empty API keys, malformed arguments and invalid requests must not change state.
 auto oldKey=apiKey;apiKey="";web.args.clear();assert(!auth());apiKey=oldKey;
 for(auto bad:{"-1","4","1abc","256"," 1",""}){
  web.args={{"mode","invalid"},{"key",apiKey},{"level",bad}};String before=mode;control();assert(web.code==400&&mode==before);
 }
 // A repeated boost carrying UI parameters must not change its timer or saved settings.
 level=2;humidity=3;night=true;request("wrg");request("boost");tick(fakeNow+500);bus.out.clear();tick(fakeNow+10000);bus.out.clear();auto savedBoost=boostStart;
 web.args={{"mode","boost"},{"level","1"},{"night","0"},{"key",apiKey}};control();assert(boostStart==savedBoost&&!waiting&&night);
 tick(savedBoost+1200000);bus.out.clear();assert(mode=="wrg"&&level==2&&humidity==3&&night);
 // Never accept missing STX, short, non-binary or overflowing response bodies.
 for(auto body:{std::string("0010110000")+char(3),std::string(1,2)+"0010"+char(3),std::string(1,2)+"00x0110000"+char(3),std::string(1,2)+std::string(100,'0')+char(3)}){
  web.args={{"key",apiKey}};web.routes["/api/query"]();bus.out.clear();auto last=lastReply;bus.in=body;tick(fakeNow+100);assert(lastReply==last);tick(fakeNow+3000);bus.out.clear();
 }
 // Bus lock requires selecting off; API gives an inline error instead of redirect.
 request("wrg");web.args={{"enabled","0"},{"key",apiKey}};web.routes["/enable"]();assert(web.code==409&&enabled);
 request("off");web.args={{"enabled","0"},{"key",apiKey}};web.routes["/enable"]();assert(web.code==409);tick(fakeNow+500);bus.out.clear();web.routes["/enable"]();assert(web.code==200&&!enabled);
 // WLAN validation and nonblocking save: no restart, immediate connection attempt.
 web.args={{"ssid","home"},{"password","short"},{"key",apiKey}};web.routes["/wifi"]();assert(web.code==400);
 web.args={{"ssid","home"},{"password","examplepass"},{"key",apiKey}};web.routes["/wifi"]();assert(web.code==200&&WiFi.ssid=="home");
 apWindowExpired=false;startAP();WiFi.connection=1;auto apTime=apStarted;tick(apTime+60000);assert(!apActive&&WiFi.apStops>0);
 WiFi.connection=0;tick(fakeNow+1);tick(fakeNow+60000);assert(apActive);
 tick(apStarted+600000);assert(!apActive&&apWindowExpired);tick(fakeNow+60000);assert(!apActive);
 assert(elapsed(500,0xfffffff0,500));assert(!elapsed(10,0xfffffff0,500));
 std::cout<<"PASS: real firmware handlers/loop: disabled output, auth, validation, WRG timing, full motor pause, repeated commands, boost expiry, status parsing, 3-second silence, stop and millis wrap\n";
}
