#pragma once
struct MDNSClass {int starts=0;bool begin(const char*){starts++;return true;}void addService(const char*,const char*,int){}};
inline MDNSClass MDNS;
