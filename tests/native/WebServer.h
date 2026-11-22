#pragma once
#define HTTP_GET 0
#define HTTP_POST 1
enum {UPLOAD_FILE_START,UPLOAD_FILE_WRITE,UPLOAD_FILE_END,UPLOAD_FILE_ABORTED};
struct HTTPUpload {int status=0;String filename;size_t currentSize=0;uint8_t buf[1436];};
class WebServer {
public:
 std::map<std::string,String> args;
 std::map<std::string,std::function<void()>> routes,uploads;
 int code=0;String body;HTTPUpload up;bool authOk=false;int authRequests=0;
 WebServer(int){}
 String arg(const char*k){return args.count(k)?args[k]:String("");}
 String header(const char*){return "";}
 void send(int c,const char* ="",String b=""){code=c;body=b;}
 void collectHeaders(const char**,int){}
 bool authenticate(const char*,const char*){return authOk;}
 void requestAuthentication(){authRequests++;code=401;}
 HTTPUpload&upload(){return up;}
 void on(const char*p,int,std::function<void()>fn){routes[p]=fn;}
 void on(const char*p,int,std::function<void()>fn,std::function<void()>ufn){routes[p]=fn;uploads[p]=ufn;}
 void begin(){}void handleClient(){}
};
