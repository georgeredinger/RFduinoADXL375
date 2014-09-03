#include <Arduino.h>

void writeTo(int device, byte address, byte val);
void readFrom(int device, byte address, int num, byte buff[]);
