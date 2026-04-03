//
//
#ifndef PWM_HPP
#define PWM_HPP

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

extern Adafruit_PWMServoDriver pwm;//sets the i2c adress to the boards defualt 
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600


#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

extern uint8_t Servo_Pan;// int used to set the servo for the first arm joint
extern uint8_t Servo_Tilt;// int used to set the servo for the second arm joint
extern uint8_t Servo_Cam1;// int used to set the servo for the third arm joint

extern int Pan_Angle;
extern int Tilt_Angle;

void SetUp_PWM();
void moveServo(int servo_ARM, float angle);

#endif