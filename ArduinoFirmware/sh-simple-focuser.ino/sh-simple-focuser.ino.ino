#include <AccelStepper.h>

constexpr auto DEVICE_GUID = "906335CB-A62C-4134-B41E-7C2A4CF44D0C";

// Creates an instance
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
AccelStepper myStepper(AccelStepper::FULL4WIRE, 2, 4, 3, 5);

bool isConnected = false;

void setup() {
  Serial.begin(57600, SERIAL_8N1);
  Serial.setTimeout(100);
  while (!Serial);
  Serial.println("INITIALIZED#");

  myStepper.setCurrentPosition(0);
	myStepper.setMaxSpeed(200.0);
  myStepper.disableOutputs();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('#');
    String response = processCommand(command);
    Serial.println(response);
  }

  myStepper.runSpeedToPosition();
}

String processCommand(String command) {
  char commandCode = command[0];

  String stepsString = command.substring(1);
  int stepsNumber = stepsString.toInt();
  bool isRunning = myStepper.isRunning() && myStepper.distanceToGo() != 0;

  switch(commandCode) {
    // CONNNECT
    case 'C':
      myStepper.enableOutputs();
      isConnected = true;
      return "OK#";
    // DISCONNECT
    case 'D':
      if(isRunning) {
        myStepper.setSpeed(0);
        myStepper.move(0);
        myStepper.runToPosition();
      }
      isConnected = false;
      myStepper.disableOutputs();
      return "OK#";
    // FORWARD steps
    case 'F':
      if(!isConnected) { return "NOK#"; }
      myStepper.move(-stepsNumber);
      myStepper.setSpeed(200);
      return "OK#";
    // BACKWARD steps
    case 'B':
      if(!isConnected) { return "NOK#"; }
      myStepper.move(stepsNumber);
      myStepper.setSpeed(200);
      return "OK#";
    // STOP
    case 'S':
      if(!isConnected) { return "NOK#"; }
      myStepper.setSpeed(0);
      myStepper.move(0);
      myStepper.runToPosition();
      return "OK#";
    // PING
    case 'P':
      return "OK:" + String(DEVICE_GUID) + "#";
    // STATUS (aka WHAT)
    case 'W':
      if(!isConnected) { return "NOK#"; }
      return isRunning ? "MOVING#" : "IDLE#";
    default:
      return "UNKNOWN#";
  }
}