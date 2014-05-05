#include "ADXL375.h"
 volatile boolean red,yellow;

void setupFIFO(){
  //   writeTo(DEVICE, 0x2D, 0);      
  //  writeTo(DEVICE, 0x2D, 16);
  //9.1 active low interrupt.  

  writeTo(DEVICE,ADXL375_DATA_FORMAT_REG,0b00011011);

  //1. Write 0x28 to Register 0x1D; set shock threshold to 31.2 g.
  // writeTo(DEVICE,0x1D,0x28); // 2,5 Gs for an ADXL345,(OX28/0xFF)*FSR
  writeTo(DEVICE,0x1D,64); // 64/255 of fsr or 50 Gs since FSR == 200 Gs
  //2. Write 0x50 to Register 0x21; set shock duration to 50 ms.
  writeTo(DEVICE,0x21,0x50);
  //3. Write 0x20 to Register 0x22; set latency to 40 ms.
  //
  writeTo(DEVICE,0x22,0x20);
  //4. Write 0xF0 to Register 0x23; set shock window to 300 ms.
  writeTo(DEVICE,0x23,0x1F);
  //5. Write 0x07 to Register 0x2A to enable X-, Y-, and Z-axes
  // participation in shock detection.
  writeTo(DEVICE,0x2A,0x07);
  //6. Write 0x0F to Register 0x2C to set output data rate to
  //  3200 Hz.
  //writeTo(DEVICE,0x2c,0X0F);
  //6. Write 0x01Dto Register 0x2C to set output data rate to
  //  800 Hz power save mode
//  writeTo(DEVICE,0x2c,0X1D);
   //6. Write 0x01E to Register 0x2C to set output data rate to
  //  1600 Hz power save mode
  writeTo(DEVICE,0x2c,0X1E);
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
  writeTo(DEVICE,0x38,0XEA);


  //10. Write 0x08 to Register 0x2D to start the measurement.
  //  It is recommended that the POWER_CTL register be
  //  configured last.

  writeTo(DEVICE,0x2D,0x08);
}

int maxFIFO() {
  int x, y, z;
  unsigned maxx=0,maxy=0,maxz=0;
  unsigned maxmax=0;
  int i=0;
    readFrom(DEVICE,0X30,1,buff);
    for(i=0;i<32;i++){
      readFrom(DEVICE, 0X32, TO_READ, buff); //read the acceleration data from the ADXL345
      x = (((int)buff[1]) << 8) | buff[0];   
      y = (((int)buff[3])<< 8) | buff[2];
      z = (((int)buff[5]) << 8) | buff[4];
      maxx=max(abs(x),maxx);
      maxy=max(abs(y),maxy);
      maxz=max(abs(z),maxz);
    }
    maxmax=max(maxx,maxy);
    maxmax = max(maxmax,maxz);
    return(maxmax);

 }
void readXYZ(int *x,int *y,int *z){

  readFrom(DEVICE, 0X32, TO_READ, buff); //read the acceleration data from the ADXL345
  *x = (((int)buff[1]) << 8) | buff[0];   
  *y = (((int)buff[3])<< 8) | buff[2];
  *z = (((int)buff[5]) << 8) | buff[4];

}

