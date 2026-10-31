#pragma once
#define HTTP_GET 0
#define HTTP_POST 1
class WebServer {
public:
 std::map<std::string,String> args;
 std::map<std::string,std::function<void()>> routes;
 int code=0;
 WebServer(int){}
 String arg(const char*k){return args[k];}
 String header(const char*){return "";}
 void send(int c,const char* ="",String =""){code=c;}
 void sendHeader(const char*,const char*){}
 void collectHeaders(const char**,int){}
 void on(const char*p,int,std::function<void()>fn){routes[p]=fn;}
 void begin(){}void handleClient(){}
};
