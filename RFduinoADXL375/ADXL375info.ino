#include "ADXL375.h"

int getID() { 
  byte id;
  readFrom(DEVICE, ADXL375_DEVID_REG, 1, &id);
  return (int) id;
}

void enableADXL() {
  writeTo(DEVICE, 0x2D, 0);      
  writeTo(DEVICE, 0x2D, 16);
  writeTo(DEVICE, 0x2D, 8);
}
void setsamplerate(){
//  writeTo(DEVICE,0x2c,0X0F); //3200 hz
//  writeTo(DEVICE,0x2c,0x0A);//100hz
}

int getSampleRate(){
  byte rate;
    readFrom(DEVICE,ADXL375_BW_RATE_REG,1,&rate );
    return (rate & 0x0F);

}


void printXYZ(){
  short x,y,z;
  readXYZ(&x,&y,&z);
  scaleXYZ(&x,&y,&z);
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z);
}


//Because the measured output for each axis is expressed in LSBs, 
//XST, YST, and ZST are also expressed in LSBs. These values can be 
//converted to acceleration (g) by multiplying each value by the 
//49 mg/LSB scale factor 
// meaning, multiply each measurement by 49e-3, or in integer math divide by 1/49e-3, or 1/.0049 or ~ 20 
// confirming this, given 49mG/LSB (.049 G/ LSB) FSR 200 G, 200/.049 == 4081 , or very close to 14 bits (plus sign)
// 4095 (14 bits) / 200 (FSR) == 20 , so dividing the data registers (x,7,z by 20 should give Gs)
#define GDIVISOR 20

void scaleXYZ(short *x,short *y,short *z) {
      *x = *x/GDIVISOR,
      *y = *y/GDIVISOR, 
      *z = *z/GDIVISOR;  
}

