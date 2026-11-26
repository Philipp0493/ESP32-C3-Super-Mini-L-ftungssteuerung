#pragma once
#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF
struct UpdateClass {
 size_t written=0;bool begun=false,ended=false,aborted=false;
 bool begin(size_t){begun=true;written=0;ended=aborted=false;return true;}
 size_t write(uint8_t*,size_t n){written+=n;return n;}
 bool end(bool){ended=true;return true;} void abort(){aborted=true;}
 bool hasError(){return false;} const char*errorString(){return "none";}
};
inline UpdateClass Update;
