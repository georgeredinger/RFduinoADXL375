#include "ADXL375.h"
#include "BitTwiddle.h"

 volatile boolean red,yellow;


//fifo data rate bits
//writeTo(DEVICE,0x2c,hex);

//hex, bits, (Hz),bw,uA
//
//
//FF 1111 3200 1600 145 
//
//0E 1110 1600 800 90 
//
//0D 1101 800 400 140 
//
//0C 1100 400 200 140 
//
//0B 1011 200 100 140 
//
//0A 1010 100 50 140 
//
//09 1001 50 25 90 
//
//08 1000 25 12.5 60 
//
//07 0111 12.5 6.25 50 
//
//06 0110 6.25 3.13 40 
//
//05 0101 3.13 1.56 35 
//
//04 0100 1.56 0.78 35 
//
//03 0011 0.78 0.39 35 
//
//02 0010 0.39 0.20 35 
//
//01 0001 0.20 0.10 35 
//
//00 0000 0.10 0.05 35 

//trigger threshold register settings
//THRESH_SHOCK register 
//The scale factor is 780 mg/LSB. A
#define FIFTYG ((10*1000)/780)

void setThreshold(short threshold){
  //writeTo(DEVICE,0x1D,(threshold*1000)/780); // 64/255 of fsr or 50 Gs since FSR == 200 Gs
    writeTo(DEVICE,0x1D,32); // 64/255 of fsr or 50 Gs since FSR == 200 Gs

}
//  writeTo(DEVICE,0x1D,FIFTYG); // 64/255 of fsr or 50 Gs since FSR == 200 Gs
#define GSCALE_DIVISOR (1000/49)

void setTriggerPosition(short pre){
 //+_____________________________________________+
//|D7  D6    |   D5    |      D4  D3  D2  D1  D0 |
//|FIFO_MODE | Trigger |     Samples             |
//+______________________________________________+
//FiFo Mode:
// 00 bypas
// 01 FiFo
// 10 Stream
// 11 Trigger
//
//Trigger set to enable trigger interuppt
//
//Samples 0..32 (00000..11111


  writeTo(DEVICE,ADXL375_FIFO_CTL,B8(11100000) | pre); 
}

void setRate(int rate){
//  Register 0x2C—BW_RATE (Read/Write)
//+----------------------------------------+
//|D7  D6  D5 | D4         |D3  D2  D1  D0 |
//|0   0    0 |  LOW_POWER | Rate          |
//+----------------------------------------+
 short rcode;
switch(rate) {
 case 3200:// 1111  15
 rcode = 15;
   break;
 case 1600:// 1110  14
 rcode = 14;
   break;
 case 800: // 1101  13
 rcode = 13;
   break;
 case 400: // 1100  12
 rcode = 12;
   break;
 case 200: // 1011  11
 rcode = 11;
   break;
 case 100: // 1010  10
 rcode = 10;
   break;
 case 50: // 1001  9
 rcode = 9;
   break;
 case 25: // 1000  8
 rcode = 8;
   break;
 case 12: // 0111  7
 rcode = 7;
   break;
 case 6: // 0110  6
 rcode = 6;
   break;
 case 3: // 0101  5
 rcode = 5;
   break;
 case 2: // 0100  4
 rcode = 4;
   break;
 case 1: // 0011  3
   rcode = 3;
   break;
  default: 
  rcode = 10;
 // 0010  0.39 2 
// 0001  0.20 1 
// 0000  0.10 0
} 
  Serial.println(rcode);
  writeTo(DEVICE,ADXL375_BW_RATE_REG,B8(10000) | rcode); 
}

//  writeTo(DEVICE,ADXL375_BW_RATE_REG,0X0E); //1600 HZ
void setupFIFO(){
  //   writeTo(DEVICE, 0x2D, 0);      
  //  writeTo(DEVICE, 0x2D, 16);
  //9.1 active low interrupt.  

  writeTo(DEVICE,ADXL375_DATA_FORMAT_REG,0b00011011);

  //1. Write 0x28 to Register 0x1D; set shock threshold to 31.2 g.
  // writeTo(DEVICE,0x1D,0x28); // 2,5 Gs for an ADXL345,(OX28/0xFF)*FSR
  setThreshold(50);
  //2. Write 0x0F to Register 0x21; set shock duration to 10 ms.
  //  625 uS/LSB.
  //  10ms, 10/0.625 = 
  writeTo(DEVICE,ADXL375_DUR_REG,0x0F);
  
  //3. Write 0 to Register 0x22; to disable second shock  latency 
  writeTo(DEVICE,0x22,0);
  //4. Write 0xF0 to Register 0x23; set shock window to 300 ms.
  writeTo(DEVICE,0x23,0x1F);
  //5. Write 0x07 to Register 0x2A to enable X-, Y-, and Z-axes
  // participation in shock detection.
  writeTo(DEVICE,0x2A,0x07);
  
  //6. Write 0x0F to Register 0x2C to set output data rate to
  setRate(1600);
  setTriggerPosition(16);

  //6. Write 0x01Dto Register 0x2C to set output data rate to
  //  800 Hz power save mode
  //writeTo(DEVICE,0x2c,0X1D);
   //6. Write 0x01E to Register 0x2C to set output data rate to
  //  1600 Hz power save mode
  //writeTo(DEVICE,0x2c,0X1E);
  //7. Write 0x40/0x20 to Register 0x2E to enable single shock or
  //  double shock, respectively.
  writeTo(DEVICE,0x2E,0x40);
  //8. Write 0x40/0x20 to Register 0x2F to assign single shock or
  //  double shock interrupt, respectively, to the INT2 pin.
  writeTo(DEVICE,0x2F,0X40);
  //9.  Write 0xEA to Register 0x38 to enable triggered mode
  //FIFO. If an interrupt is detected on the INT2 pin, the FIFO
  //records the trigger event acceleration with 10 samples
  //retained from before the trigger event.
//  Register 0x38—FIFO_CTL (Read/Write)


  //10. Write 0x08 to Register 0x2D to start the measurement.
  //  It is recommended that the POWER_CTL register be
  //  configured last.

  writeTo(DEVICE,0x2D,0x08);
}

int maxFIFO() {
  short x, y, z;
  unsigned maxx=0,maxy=0,maxz=0;
  unsigned maxmax=0;
  int i=0;
    readFrom(DEVICE,0X30,1,buff);
    for(i=0;i<32;i++){
      readFrom(DEVICE, 0X32, TO_READ, buff); //read the acceleration data from the ADXL345
      x = (((short)buff[1]) << 8) | buff[0];   
      y = (((short)buff[3])<< 8) | buff[2];
      z = (((short)buff[5]) << 8) | buff[4];
      maxx=max(abs(x),maxx);
      maxy=max(abs(y),maxy);
      maxz=max(abs(z),maxz);
    }
    maxmax=max(maxx,maxy);
    maxmax = max(maxmax,maxz);
    return(maxmax);

 }
 
 void dumpFIFO(){
   int i=0;
     short x, y, z;

    readFrom(DEVICE,ADXL375_INT_SOURCE_REG,1,buff);
    for(i=0;i<32;i++){
      readFrom(DEVICE, ADXL375_DATAX0_REG, TO_READ, buff); //read the acceleration data from the ADXL345
      x = (((short)buff[1]) << 8) | buff[0];   
      y = (((short)buff[3])<< 8) | buff[2];
      z = (((short)buff[5]) << 8) | buff[4];
      //sprintf(str, "%d %d %d", x/GSCALE_DIVISOR, y/GSCALE_DIVISOR, z/GSCALE_DIVISOR);  
      sprintf(str, "%+03d,%+03d,%+03d", x, y, z);  

      Serial.print(str);
      Serial.write(10);

    }
 }
 
void readXYZ(short *x,short *y,short *z){

  readFrom(DEVICE, 0X32, TO_READ, buff); //read the acceleration data from the ADXL345
  *x = (((short)buff[1]) << 8) | buff[0];   
  *y = (((short)buff[3])<< 8) | buff[2];
  *z = (((short)buff[5]) << 8) | buff[4];

}

