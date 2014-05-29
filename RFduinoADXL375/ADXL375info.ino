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
  writeTo(DEVICE,0x2c,0x0A);//100hz
}


void printXYZ(){
  short x,y,z;
  readXYZ(&x,&y,&z);
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z);
}
