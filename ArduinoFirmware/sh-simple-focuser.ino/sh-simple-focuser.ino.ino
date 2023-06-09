#include <AccelStepper.h>

#define MotorInterfaceType 8

// Creates an instance
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
AccelStepper myStepper(MotorInterfaceType, 2, 4, 3, 5);

bool isMoving = false;
bool isConnected = false;

void setup() {
  Serial.begin(57600, SERIAL_8N1);
  Serial.setTimeout(100);
  while (!Serial);
  Serial.println("INITIALIZED#");

  myStepper.setCurrentPosition(0);
	myStepper.setMaxSpeed(200.0);
	myStepper.setSpeed(200);
  myStepper.disableOutputs();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('#');
    String response = processCommand(command);
    Serial.println(response);
  }

  if(isMoving) {
    if (myStepper.distanceToGo() != 0) {
      myStepper.runSpeed();
    } else {
      isMoving = false;
    }
  }
}

String processCommand(String command) {
  char commandCode = command[0];

  String stepsString = command.substring(1);
  int stepsNumber = stepsString.toInt();

  switch(commandCode) {
    // CONNNECT
    case 'C':
      myStepper.enableOutputs();
      isConnected = true;
      return "OK#";
    // DISCONNECT
    case 'D':
      if(isMoving) {
        isMoving = false;
        myStepper.stop();
        myStepper.move(0);
        myStepper.runToPosition();
      }
      isConnected = false;
      myStepper.disableOutputs();
      return "OK#";
    // FORWARD steps
    case 'F':
      if(!isConnected) { return "NOK#"; }
      isMoving = true;
      myStepper.move(stepsNumber);
      myStepper.setSpeed(200);
      return "OK#";
    // BACKWARD steps
    case 'B':
      if(!isConnected) { return "NOK#"; }
      isMoving = true;
      myStepper.move(-stepsNumber);
      myStepper.setSpeed(-200);
      return "OK#";
    // STOP
    case 'S':
      if(!isConnected) { return "NOK#"; }
      isMoving = false;
      myStepper.stop();
      myStepper.move(0);
      myStepper.runToPosition();
      return "OK#";
    // STATUS (aka WHAT)
    case 'W':
      if(!isConnected) { return "NOK#"; }
      return myStepper.isRunning() && isMoving ? "MOVING#" : "IDLE#";
    default:
      return "UNKNOWN#";
  }
}