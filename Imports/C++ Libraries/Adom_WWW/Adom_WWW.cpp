#include "Adom_WWW.h"

/******************************************************************************************************************
 ******************************************************************************************************************
 ** Adom_WWW class implementation
 ******************************************************************************************************************
 *****************************************************************************************************************/

/******************************************************************************************************************
 * [CONSTRUCTOR] Adom_WWW() - Called when a new Adom_WWW object is initiated
 * @return void - no return value needed
 *****************************************************************************************************************/
Adom_WWW::Adom_WWW( char* _network , char* _key ) {
  this->Network(_network , _key);
  Serial.println("Delaying before WiFi init...");
  delay(this->wifi_delay);   //Delay needed before calling the WiFi.begin
  this->Connect();
}

/******************************************************************************************************************
 * Adom_WWW::Get() - Returns output from a HTTP GET request
 * @param char* _uri - The URI you wish to request
 * @param char* _data - The data you wish to send to the server with your request
 * @return String - HTTP request output string
 *****************************************************************************************************************/
String Adom_WWW::Get ( char* _uri , char* _data ) {
  return this->HTTP(_uri , _data , true);
}

/******************************************************************************************************************
 * Adom_WWW::Get() - Returns output from a HTTP POST request
 * @param char* _uri - The URI you wish to request
 * @param char* _data - The data you wish to send to the server with your request
 * @return String - HTTP request output string
 *****************************************************************************************************************/
String Adom_WWW::Post( char* _uri , char* _data ) {
  return this->HTTP(_uri , _data , false);
}

/******************************************************************************************************************
 * Adom_WWW::HTTP() - Returns output from a HTTP request
 * @param char* _uri - The URI you wish to request
 * @param char* _data - The data you wish to send to the server with your request
 * @param bool  _get  - true if you want to use GET false if you want to use POST
 * @return String - HTTP request output string
 *****************************************************************************************************************/
String Adom_WWW::HTTP( char* _uri , char* _data , bool _get ) {
  if ( !((WiFi.status() == WL_CONNECTED)) )
    return "Adom_WWW::HTTP - Not connected";

  HTTPClient http;
  String http_output;

  http.begin(_uri);
  int http_status = ( _get ) ? http.GET() : http.POST(_data);

  if ( http_status )
    http_output = http.getString();
  else
    Serial.println("Error on HTTP request");

  http.end(); //Free the resources
  return http_output;
}


/******************************************************************************************************************
 * Adom_WWW::Network() - Sets the WiFi network credentials
 * @param char* _network - The WiFi network you wish to use
 * @param char* _key - The WiFi password you wish to use
 * @return void - no return value necessary
 *****************************************************************************************************************/
void Adom_WWW::Network( char* _network , char* _key ) {
  this->wifi_network = _network;
  this->wifi_password = _key;
}

/******************************************************************************************************************
 * Adom_WWW::Connect() - Connects to the internet (Ethernet or WiFi)
 * @return void - no return value necessary
 *****************************************************************************************************************/
void Adom_WWW::Connect() {
  this->Connect_WiFi();
}

/******************************************************************************************************************
 * Adom_WWW::Connect_WiFi() - Connects to the internet via WiFi
 * @return void - no return value necessary
 *****************************************************************************************************************/
void Adom_WWW::Connect_WiFi() {
  Serial.print("Attempting connection @ ");
  Serial.println(this->wifi_network);
  WiFi.begin(this->wifi_network, this->wifi_password);

  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  WiFi.begin(this->wifi_network, this->wifi_password);
  this->Connect_WiFi_Persist();
  Serial.println("Connected to the WiFi network");
}

/******************************************************************************************************************
 * Adom_WWW::Connect_WiFi_Persist() - Persists a loop until a WiFi connection is established
 * @return void - no return value necessary
 *****************************************************************************************************************/
void Adom_WWW::Connect_WiFi_Persist() {
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
}


/******************************************************************************************************************
 * Adom_WWW::Connect_Ethernet() - Connects to the internet via Ethernet
 * @return void - no return value necessary
 *****************************************************************************************************************/
void Adom_WWW::Connect_Ethernet() {

}
