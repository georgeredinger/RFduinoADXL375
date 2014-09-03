
/*
Read ADXL375 accelerometer and send to BLE master

This sketch sends 20 20 byte packets to

*/

#include "ADXL375.h"
#include "Pins.h"
#include <Wire.h>
#include "i2c.h"
#include "ADXL375info.h"
#include "FiFo.h"


#include <RFduinoBLE.h>

ADXL375 shock;
int packets = 20;

// flag used to start sending
int flag = false;

int ch;
int packet;

int start;
volatile int led;

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
 pinMode(2,INPUT);
 pinMode(RED,OUTPUT);
 pinMode(GREEN,OUTPUT);
  Wire.begin();
shock.initialize();
// join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
  
  
  //Turning on the ADXL345
  writeTo(DEVICE, 0x2D, 0);      
  writeTo(DEVICE, 0x2D, 16);
  writeTo(DEVICE, 0x2D, 8);
  delay(500);

  printXYZ();

  delay(1000);
  RFduino_pinWake(2, HIGH); // configures pin  to wake up device on a high signal 
  digitalWrite(2,LOW); 
  RFduino_resetPinWake(2); // reset state of pin that caused wakeup 
  
  if(shock.testConnection()) {
    Serial.println("ADXL375 good");
    shock.setupFIFO();
    shock.dumpSetup();

  }else{
    Serial.println("ADXL375 bad");
    //set an error flag and blink funny (TODO:)
  }

  led=GREEN;

}

void RFduinoBLE_onConnect() {
  packet = 0;
  ch = 'A';
  start = 0;
  flag = true;
}

int triggers=0;
bool shocked = false;
void loop() {

  short x, y, z;
  RFduino_ULPDelay(2000); // Stay in ultra low power mode until interrupt from the BLE or pinWake() 
  
  if (RFduino_pinWoke(2)){
    RFduino_resetPinWake(2); // reset state of pin that caused wakeup 
     Serial.println(triggers++);
     shocked=true;
    shock.saveFIFO();
    led=RED;
  }else{

    digitalWrite(led,HIGH);
    RFduino_ULPDelay(50);
    digitalWrite(led,LOW);
    if(shocked) {
      shocked = false;
      shock.printFIFO();
    }
  }
  
delay(10);   
 
  


  //It appears that delay is needed in order not to clog the port
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

