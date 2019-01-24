#include "remote.h"
#include "motors.h"

#define CLOSE 0
#define FAR 1

int sensorPin = 12;

void setup() {
  Serial.begin(115200);
  motorSetup();
  pinMode(sensorPin, INPUT);
  remoteSetup("Klein", "password");
}

void loop() {
  int state = digitalRead(sensorPin);
  remoteUpdate(!state);
  if (state == FAR) {
    float left = joystickY + joystickX;
    float right = joystickY - joystickX;
    if (left > 1) left = 1;
    if (left < -1) left = -1;
    if (right > 1) right = 1;
    if (right < -1) right = -1;
    leftMotor(left * 1024);
    rightMotor(right * 1024);
  } else {
    leftMotor(-1024);
    rightMotor(-1024);
  }
}
