#include <Adom.h>
Adom* Controller;
Adom_Joystick* Joystick;

// Define onboard LED
int DebugLED = 2;

// Define joystick pins
int Joystick_Pin_X    =   A0;
int Joystick_Pin_Y    =   A3;
int Joystick_Pin_SW   =   22;



  





// main.setup
void setup() {

  // Setup the Adom global controller
  Controller = new Adom();
  // Setup the DebugLED Pin
  Controller->_Pin(DebugLED, OUTPUT);
  // Setup WiFi for IO
  Controller->_IO("LiWiFi 2.4Ghz", "Lindsey7047");

  // Setup the joystick
  Joystick = new Adom_Joystick();
  // Calibrate the deadzone for the joystick
  Joystick->Calibrate( Joystick_Pin_X , Joystick_Pin_Y , Joystick_Pin_SW );

  Serial.print("Deadzones: "); 
  Serial.print(Joystick->DeadzoneX()); 
  Serial.print(" , "); 
  Serial.println(Joystick->DeadzoneY());

  Serial.println("\n\nStanding by...\n\n");
}

// main.loop
int detections = 0;
void loop() {
  if ( Joystick->Movement() ) {
    Serial.print("["); Serial.print(++detections); Serial.print("]");
    Serial.println(" Joystick movement detected ");
  }
}


// _perform makes it perform for a full second
void Blink(int _pin , int _bps , bool _perform = false) {
  int _delay = 1000 / 2 / _bps;
  int _performances = ( _perform ) ? _bps : 1;
  for(int _i = 0; _i < _performances; _i++) {
    delay(_delay);
    Controller->Pin(DebugLED, -1); // Invert power for this pin
    delay(_delay);
    Controller->Pin(DebugLED, -1); // Invert power for this pin
  }  
}


void StandbyBlink() {
  Blink( DebugLED , 3 , true );
  Blink( DebugLED , 1 , true );
  Blink( DebugLED , 1 , true );
}



void JoystickInfo() {

  Serial.print("Switch: "); 
  Serial.print(digitalRead(Joystick_Pin_SW));
  Serial.print("\n");
  Serial.print("X : "); 
  Serial.print(analogRead(Joystick_Pin_SW));
  Serial.print("\n");
  Serial.print("Y : "); 
  Serial.print(analogRead(Joystick_Pin_SW));
  Serial.print("\n");
  
}









