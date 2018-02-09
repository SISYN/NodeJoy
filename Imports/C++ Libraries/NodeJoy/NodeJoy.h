#include <WiFi.h>

#ifndef NodeJoy_H
#define NodeJoy_H


class NodeJoy {
  public:
    NodeJoy();
    char* API_KEY;
    char* NODE_ID;


  private:
    char* MAC_ADDR;
    int   ROLE;

};



#endif
