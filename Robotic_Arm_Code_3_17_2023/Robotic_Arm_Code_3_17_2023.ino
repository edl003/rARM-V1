#include <PS4Controller.h>
#include <Adafruit_PWMServoDriver.h>
#include <Dynamixel2Arduino.h>

#define DXL_SERIAL  Serial
#define DEBUG_SERIAL  Serial2
#define DXL_DIR_PIN  26

//DXL ID's set prior using "scan_dynamixel" and "id"
const uint8_t DXL_1 = 1;
const uint8_t DXL_2 = 2;
const uint8_t DXL_3 = 3;
const uint8_t DXL_4 = 4;
const uint8_t DXL_5 = 5;
const float DXL_PROTOCOL_VERSION = 2.0;

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);

//This namespace is required to use Control table item names
using namespace ControlTableItem;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define servoMIN 156 //150
#define servoMAX 460
uint8_t current_pos = servoMIN; 

int deadZone = 15; 
uint8_t joyScale = 8.0; 

void setup() {
  dxl.begin(115200);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  DEBUG_SERIAL.begin(115200);

  PS4.begin("ec:62:60:a1:c6:aa");
  Serial.println("PS4 Bluetooth Ready");

  pwm.begin();
  pwm.setPWMFreq(50);

  dxl.ping(DXL_1);
  dxl.ping(DXL_2);
  dxl.ping(DXL_3);
  dxl.ping(DXL_4);
  dxl.ping(DXL_5);

  dxl.torqueOff(DXL_1);
  dxl.torqueOff(DXL_2);
  dxl.torqueOff(DXL_3);
  dxl.torqueOff(DXL_4);
  dxl.torqueOff(DXL_5);
  dxl.setOperatingMode(DXL_1, OP_POSITION);
  dxl.setOperatingMode(DXL_2, OP_POSITION);
  dxl.setOperatingMode(DXL_3, OP_POSITION);
  dxl.setOperatingMode(DXL_4, OP_POSITION);
  dxl.setOperatingMode(DXL_5, OP_POSITION);
  dxl.torqueOn(DXL_1);
  dxl.torqueOn(DXL_2);
  dxl.torqueOn(DXL_3);
  dxl.torqueOn(DXL_4);
  dxl.torqueOn(DXL_5);
}

void loop() {
  if (PS4.isConnected()) {
    PS4.setLed(20, 20, 20); PS4.sendToController();
    int position_base = dxl.getPresentPosition(DXL_1, UNIT_DEGREE);
    int position_shoulder = dxl.getPresentPosition(DXL_2, UNIT_DEGREE);
    int position_elbow = dxl.getPresentPosition(DXL_3, UNIT_DEGREE);
    int position_forearm = dxl.getPresentPosition(DXL_4, UNIT_DEGREE);
    int position_wrist = dxl.getPresentPosition(DXL_5, UNIT_DEGREE);



    if ((PS4.LStickX() > deadZone) || (PS4.LStickX() < -deadZone)) {
      position_base = position_base + (PS4.LStickX() / joyScale);
      dxl.setGoalPosition(DXL_1, position_base, UNIT_DEGREE);
    }
    if ((PS4.LStickY() > deadZone) || (PS4.LStickY() < -deadZone)) {
      position_shoulder = position_shoulder + (PS4.LStickY() / joyScale); 
      dxl.setGoalPosition(DXL_2, position_shoulder, UNIT_DEGREE);
    }

    if ((PS4.RStickY() > deadZone) || (PS4.RStickY() < -deadZone)) {
      position_elbow = position_elbow + (PS4.RStickY() / joyScale);
      dxl.setGoalPosition(DXL_3, position_elbow, UNIT_DEGREE);
    }
    if ((PS4.RStickX() > deadZone) || (PS4.RStickX() < -deadZone)) {
      position_forearm = position_forearm - (PS4.RStickX() / joyScale / 1.5);
      dxl.setGoalPosition(DXL_4, position_forearm, UNIT_DEGREE);
    }

    if (PS4.L1()) {
      position_wrist = position_wrist + (joyScale);
      dxl.setGoalPosition(DXL_5, position_wrist, UNIT_DEGREE);
    }
    if (PS4.R1()) {
      position_wrist = position_wrist - (joyScale);
      dxl.setGoalPosition(DXL_5, position_wrist, UNIT_DEGREE);
    }

    int lefT = PS4.L2Value();
    if (PS4.Cross()) {
      current_pos = servoMIN + (((servoMAX - servoMIN) / 255) * lefT); 
    } else if (current_pos > servoMIN) {
      pwm.setPWM(9, 0, current_pos); pwm.setPWM(10, 0, current_pos); pwm.setPWM(11, 0, current_pos); pwm.setPWM(12, 0, current_pos); pwm.setPWM(13, 0, current_pos);
    } else if (PS4.Circle()){
      current_pos = servoMIN;
    } else if (current_pos == servoMIN) {
      lefT = map(lefT, 0, 255, servoMIN, servoMAX); 
      pwm.setPWM(9, 0, lefT); pwm.setPWM(10, 0, lefT); pwm.setPWM(11, 0, lefT); pwm.setPWM(12, 0, lefT); pwm.setPWM(13, 0, lefT);
    }
    
    if (PS4.Square()) {
      pwm.setPWM(9, 0, servoMAX); pwm.setPWM(13, 0, servoMAX);
      delay(1000); 
      pwm.setPWM(12, 0, servoMAX); 
      delay(1000);
      pwm.setPWM(11, 0, servoMAX); 
      delay(1000);
      pwm.setPWM(10, 0, servoMAX); 
      delay(1000);
    }
    if (PS4.Triangle()) {
      pwm.setPWM(10, 0, servoMAX); pwm.setPWM(11, 0, servoMAX); pwm.setPWM(12, 0, servoMAX); pwm.setPWM(13, 0, servoMAX);
      delay(2000);
    }
  }
}
