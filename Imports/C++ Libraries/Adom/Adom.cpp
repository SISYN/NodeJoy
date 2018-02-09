#include "Adom.h"

/******************************************************************************************************************
 ******************************************************************************************************************
 ** Adom class implementation
 ******************************************************************************************************************
 *****************************************************************************************************************/

/******************************************************************************************************************
 * [CONSTRUCTOR] Adom() - Called when a new Adom object is initiated
 * @return void - no return value needed
 *****************************************************************************************************************/
Adom::Adom( int _baud , int _debug ) {
  this->_baud(_baud);
}


/******************************************************************************************************************
 * Adom::_baud() - Sets the baud attribute and initiates the Serial monitor
 * @return void - no return value needed
 *****************************************************************************************************************/
void Adom::_baud( int _baud ) {
  this->baud = _baud;
  Serial.begin(this->baud);
  if ( this->debug > 1 ) {
    Serial.print("Adom _baud init with baud @ "); Serial.print(this->baud); Serial.print("\n");
  }
}

/******************************************************************************************************************
 * Adom::_debug() - Sets the debug attribute
 * @return void - no return value needed
 *
 * 0 = Only fatal errors
 * 1 = Fatal + Warnings
 * 2 = All (Fatal + Warnings + Notices)
 * 3 = Verbose (All + Tracebacks)
 *****************************************************************************************************************/
void Adom::_debug( int _debug ) {
  this->debug = _debug;
}

int Adom::Pin( int _pin , int _state ) {

  if ( _state == -256 ) // retrieve current pin state
    return this->pin_states[_pin];

  if ( _state == -1 ) { // inverse
    if ( this->Pin(_pin) == HIGH )
      return this->Pin(_pin, LOW);
    return this->Pin(_pin, HIGH);
  }

  if ( this->debug > 1 ) {
    Serial.print("Adom Pin called ["); Serial.print(_pin); Serial.print("] to state "); Serial.println(_state);
  }

  this->pin_states[_pin] = _state;

  digitalWrite(_pin , _state);

  return _state;
}

void Adom::_Pin( int _pin , int _function , float _voltage ) {
  pinMode(_pin, _function);
}


/******************************************************************************************************************
 * Adom::_IO() - Sets the Adom_WWW instance
 * @return void - no return value needed
 *****************************************************************************************************************/
void Adom::_IO( char* _network , char* _key ) {
    this->IO = new Adom_WWW( _network , _key );
}
