#include "SFE_MMA8452Q.h"

#include <SoftwareSerial.h>
#include <Wire.h>

MMA8452Q accel;

const int Rx = 7;
const int Tx = 6;
float x_offset;
float y_offset;
float z_offset;
bool is_up = true;
bool is_left = true;
unsigned long last_activity;

SoftwareSerial btserial(Rx, Tx);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  btserial.begin(38400);
  //  accel.init(SCALE_2G, ODR_12);
  accel.init();
  Calibrate();

  Serial.flush();

  btserial.write("\r\n+RTADDR\r\n");
  btserial.write("\r\n+STAUTO=1\r\n");
  btserial.write("\r\n+ST0AUT=1\r\n");
}

void reset_accelerometer() {
  byte c = accel.readRegister(CTRL_REG2);
  accel.writeRegister(CTRL_REG2, c | (1<<6));
  while (c & (1<<6)) {
    delay(10);
    c = accel.readRegister(CTRL_REG2);
  }
  accel.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (accel.available()) {
    accel.read();
    printCalculatedAccels();
    delay(20);
  } else {
    Serial.println("Accelerometer Failed");
    reset_accelerometer();
  }
}

void printCalculatedAccels()
{
  float x = accel.cx - x_offset;
  float y = accel.cy - y_offset;
  float z = accel.cz - z_offset;

  if (is_up == true && z >= 0.75) {
    is_up = false;
    Serial.println("Going Down");
    btserial.println(0);
    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(z);
    Serial.println();
    last_activity = millis();
  }

  if (is_up == false && z <= -0.75) {
    is_up = true;
    Serial.println("Going Up");
    btserial.println(0);
    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(z);
    Serial.println();
    last_activity = millis();
  }

  if (is_left == true && y >= 1.0) {
    is_left = false;
    Serial.println("Going left");
    btserial.println(2);
    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(z);
    Serial.println();
    last_activity = millis();
  }

  if (is_left == false && y <= -1.0) {
    is_left = true;
    Serial.println("Going right");
    btserial.println(2);
    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(z);
    Serial.println();
    last_activity = millis();
  }

  if (millis() - last_activity >= 500) {
    Serial.println("Stop");
    btserial.println(4);

    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(z);
    Serial.println();

    last_activity = millis();
  }

  /*btserial.print(x);
  btserial.print("\t");
  btserial.print(y);
  btserial.print("\t");
  btserial.print(z);
  btserial.println();
  delay(50);*/

}

void Calibrate() {
  while (!accel.available()) {
    delay(10);
  }
  accel.read();
  x_offset = accel.cx;
  y_offset = accel.cy;
  z_offset = accel.cz;
  
  Serial.println(x_offset);
  Serial.println(y_offset);
  Serial.println(z_offset);
}
