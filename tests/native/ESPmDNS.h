#pragma once
struct MDNSClass {int starts=0;bool begin(const char*){starts++;return true;}std::map<std::string,std::string> txt;void addService(const char*,const char*,int){}void addServiceTxt(const char*,const char*,const char*k,const char*v){txt[k]=v;}};
inline MDNSClass MDNS;
