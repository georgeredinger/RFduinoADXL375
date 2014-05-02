
/*
Read ADXL375 accelerometer and send to BLE master

This sketch sends 20 20 byte packets to

*/

#include "ADXL375.h"

#include <Wire.h>

#include <RFduinoBLE.h>

int packets = 20;

// flag used to start sending
int flag = false;

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

#define DEVICE (0x53)    //ADXL345 device address
#define TO_READ (6)        //num of bytes we are going to read each time (two bytes for each axis)
char str[512];

//This buffer will hold values read from the ADXL345 registers.
char values[10];
//These variables will be used to hold the x,y and z axis accelerometer values.
int x, y, z;
byte buff[TO_READ] ;    //6 bytes buffer for saving data read from the device


void setup() {
//  RFduinoBLE.begin();
 
Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  
  //Turning on the ADXL345
  writeTo(DEVICE, 0x2D, 0);      
  writeTo(DEVICE, 0x2D, 16);
  writeTo(DEVICE, 0x2D, 8);
}

void RFduinoBLE_onConnect() {
  packet = 0;
  ch = 'A';
  start = 0;
  flag = true;
}
void loop() {

  int regAddress = 0x32;    //first axis-acceleration-data register on the ADXL345
  short x, y, z;
  
  readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345
  
   //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
   //thus we are converting both bytes in to one int
  x = ((buff[1]) << 8) | buff[0];   
  y = ((buff[3])<< 8) | buff[2];
  z = ((buff[5]) << 8) | buff[4];
  
  //we send the x y z values as a string to the serial port
  sprintf(str, "%d %d %d", x, y, z);  
  Serial.print(str);
  Serial.write(10);
  
  //It appears that delay is needed in order not to clog the port
  delay(500);
//  
//  if (flag)
//  {
//    // generate the next packet
//    char buf[21];
//    sprintf(buf, "%04dEFGHIJKLMNOPQRST", packet);
//
//    // send is queued (the ble stack delays send to the start of the next tx window)
//    while (! RFduinoBLE.send(buf, 20))
//      ;  // all tx buffers in use (can't send - try again later)
//
//    if (! start)
//      start = millis();
//
//    packet++;
//    if (packet >= packets)
//    {
//      int end = millis();
//      float secs = (end - start) / 1000.0;
//      int bps = ((packets * 20) * 8) / secs;
//      while (RFduinoBLE.radioActive) ;
//      flag = false;
//    }
//  }
}

