#include <Adom_WWW.h>
// #include <Adom_Joystick.h>

#ifndef Adom_H
#define Adom_H


/******************************************************************************************************************
 ******************************************************************************************************************
 ** Adom class design
 ******************************************************************************************************************
 *****************************************************************************************************************/
class Adom {
  public:
    // Publicly accessible attributes
    Adom_WWW* IO;

    // Consructor
    Adom( int _baud = 115200 , int _debug = 0 );

    // Publicly accessible methods
    void _baud   ( int _baud );
    void _debug  ( int _debug );
    void _IO     ( char* _network = "" , char* _key = "" );
    void _Pin    ( int _pin , int _function , float _voltage = 3.3 );

    int Pin     ( int _pin , int _state = -256 );


  private:
    int baud;
    int debug;
    int pin_states[];
};







#endif
