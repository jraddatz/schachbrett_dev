#include "Energia.h"
#include "Wire.h"


void setup(){
    Wire.begin();    
    Wire.beginTransmission(0x20);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission();

}

uint8_t porta = 0;
const uint8_t DELAY = 100;
void loop(){
    
    Wire.beginTransmission(0x20);
    Wire.write(0x12);
    Wire.write(porta);
    Wire.endTransmission();
    porta ^= 0xff;
    delay(DELAY);

}