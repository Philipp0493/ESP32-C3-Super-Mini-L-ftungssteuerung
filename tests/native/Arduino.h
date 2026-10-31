#pragma once
#include <string>
#include <map>
#include <functional>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define HEX 16
#define SERIAL_8N1 0
inline uint32_t fakeNow=100;
inline uint32_t millis(){return fakeNow;}
inline void delay(uint32_t n){fakeNow+=n;}
class String:public std::string {
public:
 using std::string::string;
 String(const std::string&s):std::string(s){}
 String(unsigned long n,int base=10){char b[32];snprintf(b,32,base==16?"%lx":"%lu",n);assign(b);}
 String(unsigned int n,int base=10):String((unsigned long)n,base){}
 String(int n):std::string(std::to_string(n)){}
 String toString()const{return *this;}
 void replace(const String&a,const String&b){size_t p=0;while((p=find(a,p))!=npos){std::string::replace(p,a.size(),b);p+=b.size();}}
 bool isEmpty()const{return empty();}
 int toInt()const{return atoi(c_str());}
 String substring(size_t a,size_t b)const{return substr(a,b-a);}
 bool startsWith(const char*s)const{return rfind(s,0)==0;}
};
inline String operator+(const String&a,const String&b){return std::string(a)+std::string(b);}
inline String operator+(const String&a,const char*b){return a+String(b);}
inline String operator+(const char*a,const String&b){return String(a)+b;}
inline String operator+(const String&a,int b){return a+String(b);}
class HardwareSerial {
public:
 std::string out,in;
 HardwareSerial(int){}
 template<class...A>void begin(A...){}
 void write(uint8_t c){out+=char(c);}
 void print(const char*s){out+=s;}
 void flush(){}
 int available(){return in.size();}
 int read(){char c=in[0];in.erase(0,1);return (uint8_t)c;}
 template<class T>void println(T){}
 explicit operator bool()const{return true;}
};
inline HardwareSerial Serial(0);
struct ESPClass {uint64_t getEfuseMac(){return 0x1234;}void restart(){}};
inline ESPClass ESP;
