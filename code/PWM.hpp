//
//
#ifndef PWM_HPP
#define PWM_HPP

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();//sets the i2c adress to the boards defualt 
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600


#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

uint8_t Servo_Pan = 0; // int used to set the servo for the first arm joint
uint8_t Servo_Tilt = 1; // int used to set the servo for the second arm joint
uint8_t Servo_Cam1 = 2; // int used to set the servo for the third arm joint

int Pan_Angle = 0;
int Tilt_Angle = 0;

void SetUp_PWM();
void moveServo(int servo_ARM, float angle);

#endif