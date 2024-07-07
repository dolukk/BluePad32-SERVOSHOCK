#include <Bluepad32.h>
#include <ESP32Servo.h>

// Define Servo Pins
#define Servo1PIN 7
#define Servo2PIN 6
#define Servo3PIN 4
#define Servo4PIN 5
#define Servo5PIN 2

// Define LED Pins
#define LED1_PIN 22
#define LED2_PIN 23
#define LED3_PIN 13
 
// Variables to hold LED states
bool led1State = false;
bool led2State = false;
bool led3State = false;

ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

// Servo names
Servo Servo1;
Servo Servo2;
Servo Servo3;
Servo Servo4;
Servo Servo5;

int servoPos = 90;


void setup() {
  Serial.begin(9600);

  // Set LED pins as outputs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  // Set servo pins as outputs
  pinMode(Servo1PIN, OUTPUT);
  pinMode(Servo2PIN, OUTPUT);
  pinMode(Servo3PIN, OUTPUT);
  pinMode(Servo4PIN, OUTPUT);
  pinMode(Servo5PIN, OUTPUT);


  Servo1.attach(Servo1PIN);
  Servo2.attach(Servo2PIN);
  Servo3.attach(Servo3PIN);
  Servo4.attach(Servo4PIN);
  Servo5.attach(Servo5PIN);


  BP32.setup(&onConnectedController, &onDisconnectedController);

  //Add forgetBluetoothKeys here if needed
}

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.print("CALLBACK: Controller is connected, index=");
      Serial.println(i);
      myControllers[i] = ctl;
      foundEmptySlot = true;

      // Optional, once the gamepad is connected, request further info about the
      // gamepad.
      ControllerProperties properties = ctl->getProperties();
      char buf[80];
      sprintf(buf,
              "BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, "
              "flags: 0x%02x",
              properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
              properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
              properties.vendor_id, properties.product_id, properties.flags);
      Serial.println(buf);
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println(
        "CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.print("CALLBACK: Controller is disconnected from index=");
      Serial.println(i);
      myControllers[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
        "CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void controllerServo(ControllerPtr gamepad){

  int R2 = gamepad->throttle();
  int L2 = gamepad->brake();
  int R2L2 = map(R2 - L2, -1023, 1023, 0, 180);
  Servo1.write(R2L2);

  int AxisLX = gamepad->axisX();
  int LX = map(AxisLX, -511, 511, 0, 180);
  Servo2.write(LX);

  int AxisLY = gamepad->axisY();
  int LY = map(AxisLY, -511, 511, 0, 180);
  Servo3.write(LY);

  int AxisRX = gamepad->axisRX();
  int RX = map(AxisRX, -511, 511, 0, 180);
  Servo4.write(RX);

  int AxisRY = gamepad->axisRY();
  int RY = map(AxisRY, -511, 511, 0, 180);
  Servo5.write(RY);
  
    // Cross button - LED1 momentary control
  if (gamepad->b()) {
    Serial.println("Cross pressed");
    led1State = true;
    digitalWrite(LED1_PIN, led1State);
  }
  if (gamepad->b()) {
    Serial.println("Cross released");
    led1State = false;
    digitalWrite(LED1_PIN, led1State);
  }

    // Triangle Button - LED2 toggle control
  if (gamepad->x()) {
    Serial.println("Triangle presssed");
    led2State = !led2State;
    digitalWrite(LED2_PIN, led2State);
  }
 
  // Square Button - LED3 on
  if (gamepad->a()) {
    Serial.println("Square pressed");
    led3State = true;
    digitalWrite(LED3_PIN, led3State);
  }
 
  // Circle Button - LED3 off
  if (gamepad->y()) {
    Serial.println("Circle pressed");
    led3State = false;
    digitalWrite(LED3_PIN, led3State);
  }
}



void loop() {
  BP32.update();
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    ControllerPtr myController = myControllers[i];
    if (myController && myController->isConnected()) 
  controllerServo(myController);

    }
    delay(20);
}