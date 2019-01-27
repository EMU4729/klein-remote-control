#include "remote.h"
#include "motors.h"

#define CLOSE 0
#define FAR 1

int sensorPin = 12;

void setup() {
  Serial.begin(115200);
  motorSetup();
  pinMode(sensorPin, INPUT);
  remoteName("Robot");
  remotePassword("password");
  remoteSetup();
}

void loop() {
  int reading = digitalRead(sensorPin);
  int vibrate = 0;
  if (reading == CLOSE) {
    vibrate = 1;
  }
  remoteUpdate(vibrate);
  float forwards = joystickY;
  float sideways = joystickX;
  if (reading == CLOSE && forwards > 0) {
    forwards = 0;
  }
  float left = forwards + sideways;
  float right = forwards - sideways;
  if (left > 1) left = 1;
  if (left < -1) left = -1;
  if (right > 1) right = 1;
  if (right < -1) right = -1;
  leftMotor(left * 1024);
  rightMotor(right * 1024);
}
