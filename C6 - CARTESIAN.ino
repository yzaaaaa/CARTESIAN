#include <ESP32Servo.h>
#include <IRremote.h>
#include <DabbleESP32.h>
#include <Arduino.h>

//Infrared Receiver
const int IRPIN = 23;

//*****MOTOR_DRIVER*****//

// Front Wheels
const int FL4 = 25;
const int FL3 = 26;
const int FR1 = 27;
const int FR2 = 32;

//*****ROBOT_ARM*****//

//Declare object for 5 Servo Motors  
Servo Servo_0;
Servo Servo_1;
Servo Servo_3;
Servo Gripper;

//*****LOGIC,INTEGERS,BOOL*****//

//Last Code Received
unsigned long lastCode;

//initial angle  for servo
int angle = 0;
int angleStep = 2;

//Servo1
int baseA = 80;
//Servo2
int elbowB = 0;
//Servo3
int shoulderC = 0;
//Servo4
int gripD = 0;

//statement
bool Drive = false;
bool Robot = false;
bool Records = false;
bool Stops = false;
bool Resets = false;
bool Plays = false;


void setup() {
  Serial.begin(115200); //Serial Monitor for Debugging
  Dabble.begin("SIR MIKKO POGI");       //set bluetooth name of your device

  IrReceiver.begin(IRPIN, ENABLE_LED_FEEDBACK);

  //Declare the pins to which the Servo Motors are connected to 
  Servo_0.attach(4);
  Servo_1.attach(17);
  Servo_3.attach(19);
  Gripper.attach(22);

  //Write the servo motors to initial position 
  Servo_0.write(80);
  Servo_1.write(0);
  Servo_3.write(90);
  Gripper.write(0);

  pinMode(FL4, OUTPUT);
  pinMode(FL3, OUTPUT);
  pinMode(FR1, OUTPUT);
  pinMode(FR2, OUTPUT);
  }


void loop() {
   Dabble.processInput();

    //IR Receiver
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
  {
    if (IrReceiver.decodedIRData.decodedRawData == 0) {
      //REPEAT LAST CODE
      Serial.println("Last Code initialized");
      IrReceiver.decodedIRData.decodedRawData = lastCode;
    }
    if (IrReceiver.decodedIRData.decodedRawData == 0xBA45FF00) {
      Serial.println("Manual Driving");
      lastCode = IrReceiver.decodedIRData.decodedRawData;
      Drive = true;
      Robot = false;
      Stops = false;
      Resets = false;
     
    }
    if (IrReceiver.decodedIRData.decodedRawData == 0xB946FF00) {
      Serial.println("Manual Robot");
      lastCode = IrReceiver.decodedIRData.decodedRawData;
      Drive = false;
      Robot = true;
      Stops = false;
      Resets = false;
    }
    if (IrReceiver.decodedIRData.decodedRawData == 0xE31CFF00) {
      Serial.println("RESET");
      lastCode = IrReceiver.decodedIRData.decodedRawData;
      Drive = false;
      Robot = false;
      Stops = false;
      Resets = true;
    }
    if (IrReceiver.decodedIRData.decodedRawData == 0xE619FF00) {
      Serial.println("STOP");
      lastCode = IrReceiver.decodedIRData.decodedRawData;
      Drive = false;
      Robot = false;
      Stops = true;
      Resets = false;
    }
  }
  IrReceiver.resume();
  }
  if (Drive == true) { //If user has selected Stop Mode 
    motor();
  }

  if (Robot == true) {//If user has selected Stop Mode 
    arm();
  }

  if (Stops == true) {//If user has selected Stop Mode 
    Stop();
  }

  if (Resets == true) {//If user has selected Stop Mode 
    reset();
  }
}

//********************************MOTOR_FUNCTIONS*********************************************//

void motor() {
  int a = GamePad.getAngle();
  int b = GamePad.getRadius();
    
  //FORWARD
  if (a > 60 && a < 120) {
    goBackward();
  }
  //BACKWARD
  if (a > 240 && a < 300) {
    goForward();
  }
  //LEFT
  if (a > 150 && a < 210) {
    goLeft();
  }
  //RIGHT
  if (a > 330 || a < 30 && b != 0) {
    goRight();
  }
  //spinRight
  if (GamePad.isCirclePressed()) {
    spinRight();
  }
  if (GamePad.isSquarePressed()) {
    spinLeft();
  }
  if (b == 0) {
    stop();
  }
}

void goBackward() {
  Serial.println("Backward");
  digitalWrite(FL4, HIGH);
  digitalWrite(FL3, LOW);
  digitalWrite(FR2, HIGH);
  digitalWrite(FR1, LOW);
}

void stop() {
  digitalWrite(FL4, LOW);
  digitalWrite(FL3, LOW);
  digitalWrite(FR2, LOW);
  digitalWrite(FR1, LOW);
}

void goForward() {
  Serial.println("Forward");
  digitalWrite(FL4, LOW);
  digitalWrite(FL3, HIGH);
  digitalWrite(FR2, LOW);
  digitalWrite(FR1, HIGH);
}

void goLeft() {
  Serial.println("Left");
  digitalWrite(FL4, LOW);
  digitalWrite(FL3, HIGH);
  digitalWrite(FR2, HIGH);
  digitalWrite(FR1, LOW);
}

void goRight() {
  Serial.println("Right");
  digitalWrite(FL4, HIGH);
  digitalWrite(FL3, LOW);
  digitalWrite(FR2, LOW);
  digitalWrite(FR1, HIGH);
}

void spinLeft() {
  Serial.println("spinL");
  digitalWrite(FL4, LOW);
  digitalWrite(FL3, HIGH);
  digitalWrite(FR2, HIGH);
  digitalWrite(FR1, LOW);
}

void spinRight() {
  Serial.println("spinR");
  digitalWrite(FL4, HIGH);
  digitalWrite(FL3, LOW);
  digitalWrite(FR2, LOW);
  digitalWrite(FR1, HIGH);
}


//******************************ROBOT_ARM_FUNCTIONS*******************************************//
void arm() { //Function to read the Analog value form POT and map it to Servo value
  int a = GamePad.getAngle();
  int b = GamePad.getRadius();
   
  //FORWARD
  if (a > 60 && a < 120 || GamePad.isUpPressed()) {
    shoulderC = shoulderC + angleStep;
    Servo_1.write(shoulderC);
    if(shoulderC>=90) {
      shoulderC = 90;
      Servo_1.write(shoulderC);
    }
    delay(20);
  }
  //BACKWARD
  if (a > 240 && a < 300 || GamePad.isDownPressed()) {
    shoulderC = shoulderC - angleStep;
    Servo_1.write(shoulderC);
    if(shoulderC<=0) {
      shoulderC = 0;
      Servo_1.write(shoulderC);
    }
    delay(20);
  }
  //LEFT
  if (a > 150 && a < 220 || GamePad.isLeftPressed()) {
    baseA = baseA + angleStep;
    Servo_0.write(baseA);
    if(baseA>=180){
      baseA = 180;
      Servo_0.write(baseA);
    }
    delay(20);
  }
  //RIGHT
  if (a > 330 || a < 30 && b != 0 || GamePad.isRightPressed()) {
    baseA = baseA - angleStep;
    Servo_0.write(baseA);
    if(baseA<=0) {
      baseA = 0;
      Servo_0.write(baseA);
    }
    delay(20);
  }
  //UP DOWN
  if (GamePad.isTrianglePressed()) {
    elbowB = elbowB + angleStep;
    Servo_3.write(elbowB);
    if(elbowB>=90) {
      elbowB = 90;
      Servo_3.write(elbowB);
    }
    delay(20);
  }
  if (GamePad.isCrossPressed()) {
    elbowB = elbowB - angleStep;
    Servo_3.write(elbowB);
    if(elbowB<=0) {
      elbowB = 0;
      Servo_3.write(elbowB);
    }
    delay(20);
  }
    //Gripper
  if (GamePad.isCirclePressed()) {
    gripD = gripD + angleStep;
    Gripper.write(gripD);
    if(gripD>=90) {
      gripD = 90;
      Gripper.write(gripD);
    }
    delay(20);
  }
  if (GamePad.isSquarePressed()) {
    gripD = gripD - angleStep;
    Gripper.write(gripD);
    if(gripD<=0) {
      gripD = 0;
      Gripper.write(gripD);
    }
    delay(20);
    }
} 

void reset() {
  Servo_0.write(80);
  Servo_1.write(0);
  Servo_3.write(0);
  Gripper.write(0);
  stop();
} 
     
void Stop() {
  Servo_0.write(80);
  Servo_1.write(0);
  Servo_3.write(0);
  Gripper.write(0);
}
