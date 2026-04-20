//
//
#include "PWM.hpp"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define Servo_Pan = 0; 
#define Servo_Tilt = 1; 
#define Servo_Cam1 = 2; 

int Pan_Angle = 0;
int Tilt_Angle = 0;

void SetUp_PWM(){
  pwm.begin(); // sets up the PWM for the board

  pwm.setOscillatorFrequency(25000000); // board internal oscillator is between 23-27MHz so 25MHz creates the lowest error interval without having to calibrate to the exact oscillator value
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  xTaskCreatePinnedToCore(Servo_Task,"Servo Task",8192,NULL,1,NULL,1); // create a multithreading task pinned to core 1

  delay(10); // add a debouncing delay
}

void moveServo(int servo_ARM, float angle) {
    if (angle < 0){ // check the lower bounds of the servo angle
       angle = 0; // set the angle to 0 
       } 
    else if (angle > 180) { // check the upper bound of the servo angle
      angle = 180; // set the angle to 180
      }
    
    int pulse_length = map(angle, 0, 180, SERVOMIN, SERVOMAX); // conver the angle to a pulse length

    pwm.setPWM(servo_ARM, 0, pulse_length); // move the servo to that angle
    delay(500); // add a half a second delay to allow arm to stop in place before moving again
}

void Servo_Task(){

  While(True){

  }
}

