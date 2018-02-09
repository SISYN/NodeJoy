#ifndef Adom_Joystick_H
#define Adom_Joystick_H


/******************************************************************************************************************
 ******************************************************************************************************************
 ** Adom_Joystick class design
 ******************************************************************************************************************
 *****************************************************************************************************************/
class Adom_Joystick {
  public:
    Adom_Joystick( int _debug = 0 );
    int X(  int _pin = -1 , int _deadzone = -1 , int _delay = -1 );
    int Y(  int _pin = -1 , int _deadzone = -1 , int _delay = -1 );
    int SW( int _pin = -1 , int _delay = -1 );


    void On( char* _direction , int _deadzone = 150 , int _delay = -1 );
    void Calibrate( int _x = -1 , int _y = -1 , int _sw = -1 , int _stability = 2500 , int _delay = 500 );

    bool Center();
    bool Movement();

    bool Sync();
    bool SyncX();
    bool SyncY();
    bool SyncSW();

    int DeviationX();
    int DeviationY();

    int DeadzoneX( int _deadzone = -1 );
    int DeadzoneY( int _deadzone = -1 );


  private:
    int debug;
    int Pin_X, Pin_Y, Pin_SW;
    int Delay_X, Delay_Y, Delay_SW;
    int Value_X, Value_Y, Value_SW;
    int Deadzone_X, Deadzone_Y;

    int Max_X, Min_X, Max_Y, Min_Y, First_X, First_Y;

    int CalibrationStability, CalibrationStabilityTimer;
    int CalibrationMaxDeviation_X, CalibrationMaxDeviation_Y;


    bool Calibration();
    void CalibrationRefinement();


};






#endif
