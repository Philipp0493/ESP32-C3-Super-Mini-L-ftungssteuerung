#pragma once
struct DNSServer {int starts=0,stops=0,requests=0;bool start(int,const char*,IPAddress){starts++;return true;}void stop(){stops++;}void processNextRequest(){requests++;}};
