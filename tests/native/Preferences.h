#pragma once
#include <algorithm>
// In-Memory-Ersatz für den NVS-Speicher.
struct Preferences {
 std::map<std::string,std::string> store;int writes=0;
 void begin(const char*,bool){}
 bool has(const char*k){return store.count(k);}
 String getString(const char*k,const char*d){return has(k)?String(store[k]):String(d);}
 bool getBool(const char*k,bool d){return has(k)?store[k]=="1":d;}
 uint8_t getUChar(const char*k,uint8_t d){return has(k)?uint8_t(atoi(store[k].c_str())):d;}
 void putString(const char*k,String v){store[k]=v;writes++;}
 void putBool(const char*k,bool v){store[k]=v?"1":"0";writes++;}
 void putUChar(const char*k,uint8_t v){store[k]=std::to_string(v);writes++;}
 uint16_t getUShort(const char*k,uint16_t d){return has(k)?uint16_t(atoi(store[k].c_str())):d;}
 void putUShort(const char*k,uint16_t v){store[k]=std::to_string(v);writes++;}
 size_t getBytesLength(const char*k){return has(k)?store[k].size():0;}
 size_t getBytes(const char*k,void*b,size_t n){if(!has(k))return 0;size_t m=std::min(n,store[k].size());memcpy(b,store[k].data(),m);return m;}
 void putBytes(const char*k,const void*b,size_t n){store[k]=std::string((const char*)b,n);writes++;}
 void remove(const char*k){store.erase(k);}
};
