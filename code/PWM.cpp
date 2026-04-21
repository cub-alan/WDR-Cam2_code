//Jacob Holwill 10859926
//

#include "PWM.hpp"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int Servo_Pan = 0;
int Servo_Tilt = 1; 
int Servo_Cam1 = 2;

int Pan_Start_Angle = 70;
int Tilt_Start_Angle = 50;
int Cam1_Start_Angle = 40;

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
    delay(25); // add a half a second delay to allow arm to stop in place before moving again
}

void Servo_Task(void *param){
  while(true){
     for (int i = 0; i<40;i = i + 4){
      moveServo(Servo_Tilt, Tilt_Start_Angle+i);
      for (int j = 0; j<40;j++){
        moveServo(Servo_Pan, Pan_Start_Angle+j);
        moveServo(Servo_Cam1, 40+j);
      }
      for (int j = 40; j>1;j--){
        moveServo(Servo_Pan, Pan_Start_Angle+j);
        moveServo(Servo_Cam1, 40+j);
      }
    }
    for (int i = 40; i>1;i=i-4){
      moveServo(Servo_Tilt, Tilt_Start_Angle+i);
      for (int j = 0; j<40;j++){
        moveServo(Servo_Pan, Pan_Start_Angle+j);
        moveServo(Servo_Cam1, 40+j);
      }
      for (int j = 40; j>1;j--){
        moveServo(Servo_Pan, Pan_Start_Angle+j);
        moveServo(Servo_Cam1, 40+j);
      }
    }
  }
}

