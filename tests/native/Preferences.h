#pragma once
struct Preferences {
 void begin(const char*,bool){}
 String getString(const char*,const char*d){return d;}
 bool getBool(const char*,bool d){return d;}
 void putString(const char*,String){} void putBool(const char*,bool){}
};
