#include "Arduino.h" // Imported for digitalRead and analogRead
#include "Adom_Joystick.h"

/******************************************************************************************************************
 ******************************************************************************************************************
 ** Adom_Joystick class implementation
 ******************************************************************************************************************
 *****************************************************************************************************************/

/******************************************************************************************************************
 * [CONSTRUCTOR] Adom_Joystick() - Called when a new Adom_Joystick object is initiated
 * @return void - no return value needed
 *****************************************************************************************************************/
Adom_Joystick::Adom_Joystick( int _debug ) {
  this->debug = _debug;
  if ( this->debug )
    Serial.println("New Adom_Joystick instance created");
}


/******************************************************************************************************************
 * Adom_Joystick::CalibrationRefinement() - Refines deadzone settings after intial calibration is successful
 * @return void - No return value necessary
 *****************************************************************************************************************/
void Adom_Joystick::CalibrationRefinement() {
  // If deadzone is less than 5%, double it or increase to 5% whichever is lower
}

/******************************************************************************************************************
 * Adom_Joystick::Center() - Returns true if joystick is centered false if not (accounts for deadzone)
 * @return bool - True if joystick is centered, false if its off-center
 *****************************************************************************************************************/
bool Adom_Joystick::Center() {
  return false;
}


/******************************************************************************************************************
 * Adom_Joystick::Movement() - Returns true if movement is detected false if not (accounts for deadzone)
 * @return bool - True if movement detected, false if no movement detected
 *****************************************************************************************************************/
bool Adom_Joystick::Movement() {
  // No movement detected if Sync returns true
  // If deviation is less than deadzone, no movement has taken place
  // Could use DeadzoneX() but using member attr is technically more efficient
  if ( !this->Sync() && (   this->DeviationX() > this->Deadzone_X || this->DeviationY() > this->Deadzone_Y   ) )
    return true;


  return false;
}





/******************************************************************************************************************
 * Adom_Joystick::DeadzoneX() - Returns the Deadzone_X attribute
 * @return int - Returns the Deadzone_X attribute
 *****************************************************************************************************************/
int Adom_Joystick::DeadzoneX( int _deadzone ) {
  if ( _deadzone != -1 )
    this->Deadzone_X = _deadzone;
  return this->Deadzone_X;
}

/******************************************************************************************************************
 * Adom_Joystick::DeadzoneY() - Returns the Deadzone_Y attribute
 * @return int - Returns the Deadzone_Y attribute
 *****************************************************************************************************************/
int Adom_Joystick::DeadzoneY( int _deadzone ) {
  if ( _deadzone != -1 )
    this->Deadzone_Y = _deadzone;
  return this->Deadzone_Y;
}

/******************************************************************************************************************
 * Adom_Joystick::DeviationX() - Returns the amount of deviation in X since the last positional update
 * @return int - Returns the amount of deviation in X
 *****************************************************************************************************************/
int Adom_Joystick::DeviationX() {
  if ( this->debug ) {
    Serial.print("DeviationX : "); Serial.println(abs(this->Value_X - this->X()));
  }
  return abs(this->Value_X - this->X());
}

/******************************************************************************************************************
 * Adom_Joystick::DeviationY() - Returns the amount of deviation in Y since the last positional update
 * @return int - Returns the amount of deviation in Y
 *****************************************************************************************************************/
int Adom_Joystick::DeviationY() {
  if ( this->debug ) {
    Serial.print("DeviationY : "); Serial.println(abs(this->Value_Y - this->Y()));
  }
  return abs(this->Value_Y - this->Y());
}




/******************************************************************************************************************
 * Adom_Joystick::X() - Sets the pin,deadzone,delay for X and/or returns the current value for X
 * @param int _pin - The pin you wish to use for the X control of this joystick
 * @param int _deadzone - The amount of deadzone for X before movement is registered
 * @param int _delay - The amount of delay before X movement is registered
 * @return float - The current position of X
 *****************************************************************************************************************/
int Adom_Joystick::X ( int _pin , int _deadzone , int _delay ) {
  if ( _pin != -1 )
    this->Pin_X = _pin;
  if ( _deadzone != -1 )
    this->Deadzone_X = _deadzone;
  if ( _delay != -1 )
    this->Delay_X = _delay;

  return analogRead(this->Pin_X);
}


/******************************************************************************************************************
 * Adom_Joystick::Y() - Sets the pin,deadzone,delay for Y and/or returns the current value for Y
 * @param int _pin - The pin you wish to use for the Y control of this joystick
 * @param int _deadzone - The amount of deadzone for Y before movement is registered
 * @param int _delay - The amount of delay before Y movement is registered
 * @return float - The current position of Y
 *****************************************************************************************************************/
int Adom_Joystick::Y ( int _pin , int _deadzone , int _delay ) {
  if ( _pin != -1 )
    this->Pin_Y = _pin;
  if ( _deadzone != -1 )
    this->Deadzone_Y = _deadzone;
  if ( _delay != -1 )
    this->Delay_Y = _delay;

  return analogRead(this->Pin_Y);
}



/******************************************************************************************************************
 * Adom_Joystick::SW() - Sets the pin,delay for the joystick switch and/or returns its current state
 * @param int _pin - The pin you wish to use for the joystick switch
 * @param int _delay - The amount of delay before the joystick switch state change is registered
 * @return int - The current state of the joystick switch
 *****************************************************************************************************************/
int Adom_Joystick::SW ( int _pin , int _delay ) {
  if ( _pin != -1 )
    this->Pin_SW = _pin;
  if ( _delay != -1 )
    this->Delay_SW = _delay;

  // Configure the Joystick Pins
  pinMode(this->Pin_SW, INPUT);
  digitalWrite(this->Pin_SW, HIGH);

  return digitalRead(this->Pin_SW);
}



/******************************************************************************************************************
 * Adom_Joystick::Sync() - Syncs the new state of the joystick components X, Y, SW
 * @return bool - True if unchanged, false if changed
 *****************************************************************************************************************/
bool Adom_Joystick::Sync() {
  bool changed = ( this->SyncX() && this->SyncY() && this->SyncSW() );

  // Set values
  this->Value_X = this->X();
  this->Value_Y = this->Y();
  this->Value_SW = this->SW();

  // Set min/max values
  this->Max_X = (this->Max_X < this->Value_X) ? this->Value_X : this->Max_X;
  this->Min_X = (this->Min_X > this->Value_X) ? this->Value_X : this->Min_X;
  this->Max_Y = (this->Max_Y < this->Value_Y) ? this->Value_Y : this->Max_Y;
  this->Min_Y = (this->Min_Y > this->Value_Y) ? this->Value_Y : this->Min_Y;

  return changed;
}

/******************************************************************************************************************
 * Adom_Joystick::SyncX() - Syncs the new position of X
 * @return bool - True if unchanged, false if changed
 *****************************************************************************************************************/
bool Adom_Joystick::SyncX() {
  int deviation = this->DeviationX();
  if ( deviation > abs(this->Deadzone_X) )  {   // X has moved
    if ( this->debug > 0 ) {
      Serial.print("this->X() = ");
      Serial.print(this->X());
      Serial.print("  --  Value_X = ");
      Serial.print(this->Value_X);
      Serial.print("  --  X Deviation = ");
      Serial.print(deviation);
      Serial.print("\n");
    }
    return false;
  }
  return true;
}

/******************************************************************************************************************
 * Adom_Joystick::SyncY() - Syncs the new position of Y
 * @return bool - True if unchanged, false if changed
 *****************************************************************************************************************/
bool Adom_Joystick::SyncY() {
  int deviation = this->DeviationY();
  if ( deviation > abs(this->Deadzone_Y) )  {   // Y has moved
    if ( this->debug > 0 ) {
      Serial.print("this->Y() = ");
      Serial.print(this->Y());
      Serial.print("  --  Value_Y = ");
      Serial.print(this->Value_Y);
      Serial.print("  --  Y Deviation = ");
      Serial.print(deviation);
      Serial.print("\n");
    }
    return false;
  }
  return true;
}



/******************************************************************************************************************
 * Adom_Joystick::SyncSW() - Syncs the new state of the joystick switch
 * @return bool - True if unchanged, false if changed
 *****************************************************************************************************************/
bool Adom_Joystick::SyncSW() {
  if ( this->Value_SW != this->SW() ) {
    // Switch has changed
    if ( this->debug > 0 )
      Serial.println("Switch has changed states");
    return false;
  }

  return true;
}





/******************************************************************************************************************
 * Adom_Joystick::Calibrate() - Sets joystick components and calibrates the joystick's deadzone automatically
 * @return void - No return value necessary
 *****************************************************************************************************************/
void Adom_Joystick::Calibrate( int _x , int _y , int _sw , int _stability , int _delay ) {
  if ( this->debug )
    Serial.println("Adom_Joystick::Calibrate() called");

  // Set X , Y , SW Pins (if they're provided, if not no worries defaults are handled in X/Y/SW methods)
  this->X(_x);
  this->Y(_y);
  this->SW(_sw);

  this->CalibrationStability = _stability;
  this->CalibrationStabilityTimer = millis();


  // Record component positions without updating deviation max until delay period expires
  while ( this->CalibrationStabilityTimer + _delay > millis() )
    this->Sync();

  for(int i = 0; i < 10; i++)
    Serial.println("[Now tracking max deviations]");
  int stability_differential = millis() - this->CalibrationStabilityTimer;
  while( stability_differential < this->CalibrationStability ) {
    stability_differential = millis() - this->CalibrationStabilityTimer;
    Serial.print("Waiting on Calibration() completion with stability differential ");
    Serial.println(stability_differential);
    this->Calibration();
  }

  // Set deadzones
  this->Deadzone_X = this->CalibrationMaxDeviation_X;
  this->Deadzone_Y = this->CalibrationMaxDeviation_Y;

}

/******************************************************************************************************************
 * Adom_Joystick::Calibration() - Ran in a loop by Calibrate() until max deviation is reliably known
 * @return bool - True if no deviation change, false if max deviation had to be updated
 *****************************************************************************************************************/
bool Adom_Joystick::Calibration() {
  if ( this->debug )
    Serial.println("Adom_Joystick::Calibration() called");


  this->Sync();

  // Check if its current deviation is less than the existing max deviation
  bool deviation_reconciled = true;
  int deviation_x = this->DeviationX();
  int deviation_y = this->DeviationY();

  if ( this->debug ) {
   Serial.print("Compare X deviation max:"); Serial.print(this->CalibrationMaxDeviation_X);
   Serial.print(" to new:"); Serial.println(deviation_x);
   Serial.print("Compare Y deviation max:"); Serial.print(this->CalibrationMaxDeviation_Y);
   Serial.print(" to new:"); Serial.println(deviation_y);
  }

  if ( deviation_x > this->CalibrationMaxDeviation_X ) {
   deviation_reconciled = false;
   this->CalibrationStabilityTimer = millis();
   this->CalibrationMaxDeviation_X = deviation_x;
  }

  if ( deviation_y > this->CalibrationMaxDeviation_Y ) {
   deviation_reconciled = false;
   this->CalibrationStabilityTimer = millis();
   this->CalibrationMaxDeviation_Y = deviation_y;
  }

  return deviation_reconciled;
}
