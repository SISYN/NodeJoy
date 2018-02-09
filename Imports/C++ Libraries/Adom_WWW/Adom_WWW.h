#include <WiFi.h>
#include <HTTPClient.h>

#ifndef Adom_WWW_H
#define Adom_WWW_H


/******************************************************************************************************************
 ******************************************************************************************************************
 ** Adom_WWW class design
 ******************************************************************************************************************
 *****************************************************************************************************************/
class Adom_WWW {
  public:
    Adom_WWW( char* _network = "" , char* _key = "" );
    String Get ( char* _uri , char* _data = "" );
    String Post( char* _uri , char* _data = "" );
    String HTTP( char* _uri , char* _data = "" , bool _get = true );

    void Network( char* _network , char* _key ); // Specify the network and password for WiFi


  private:
    HTTPClient http;
    char* wifi_network; // WiFi Network name (if using as backup from ethernet)
    char* wifi_password; // WiFi Network password (if using as backup from ethernet)
    int   wifi_delay = 4000; // WiFi Network init delay (required)

    void Init();

    void Connect();
    void Connect_WiFi();
    void Connect_Ethernet();

    void Connect_WiFi_Persist();

};






#endif
