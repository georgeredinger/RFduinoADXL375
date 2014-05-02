
/*
Read ADXL375 accelerometer and send to BLE master

This sketch sends 20 20 byte packets to

*/
//#define PIN_WIRE_SDA         (6u)
//#define PIN_WIRE_SCL         (5u)
//adxl345 i2c Address 0x53 if SDO tied to ground, 0x1D if to VCC
#include "ADXL375.h"

#include <Wire.h>

#include <RFduinoBLE.h>

int packets = 20;

// flag used to start sending
int flag = false;

// variables used in packet generation
int ch;
int packet;

int start;

char POWER_CTL = 0x2D;    //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;   //X-Axis Data 0
char DATAX1 = 0x33;   //X-Axis Data 1
char DATAY0 = 0x34;   //Y-Axis Data 0
char DATAY1 = 0x35;   //Y-Axis Data 1
char DATAZ0 = 0x36;   //Z-Axis Data 0
char DATAZ1 = 0x37;   //Z-Axis Data 1

//This buffer will hold values read from the ADXL345 registers.
char values[10];
//These variables will be used to hold the x,y and z axis accelerometer values.
int x, y, z;

void setup() {
  RFduinoBLE.begin();
 

}

void RFduinoBLE_onConnect() {
  packet = 0;
  ch = 'A';
  start = 0;
  flag = true;
}
void loop() {

  if (flag)
  {
    // generate the next packet
    char buf[21];
    sprintf(buf, "%04dEFGHIJKLMNOPQRST", packet);

    // send is queued (the ble stack delays send to the start of the next tx window)
    while (! RFduinoBLE.send(buf, 20))
      ;  // all tx buffers in use (can't send - try again later)

    if (! start)
      start = millis();

    packet++;
    if (packet >= packets)
    {
      int end = millis();
      float secs = (end - start) / 1000.0;
      int bps = ((packets * 20) * 8) / secs;
      while (RFduinoBLE.radioActive) ;
      flag = false;
    }
  }
}

