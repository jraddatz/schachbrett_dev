
#include "hardware/LCD162C.h"
#include "mbed.h"

LCD162C::LCD162C(PinName rs, PinName rw, PinName e, 
        PinName d0, PinName d1, PinName d2, PinName d3, 
        PinName d4, PinName d5, PinName d6, PinName d7) : 
        _rs(rs), _rw(rw), _e(e), 
        _d(d0, d1, d2, d3, d4, d5, d6, d7) {
    _d.output();
    _e  = 1;
    _rs = 0;            // command  mode
    _rw = 0;

    wait_ms(15);        // Wait 15ms to ensure powered up

    for (uint8_t i=0; i<3; i++) {
        writeByte(0x30);
        wait_ms(1.64);  // this command takes 1.64ms, so wait for it
    }

    writeCommand(0x28); // Function set 001 BW N F - -
    writeCommand(0x0C);
    writeCommand(0x6);  // Cursor Direction and Display Shift : 0000 01 CD S (CD 0-left, 1-right S(hift) 0-no, 1-yes
    cls();
}

void LCD162C::print(const char* str){
    uint8_t i = 0;
    while ( i < 32 && str[i]){
        writeByte(str[i]);
    }
}


void LCD162C::cls() {
    writeCommand(0x01); // cls, and set cursor to 0
    wait_ms(1.64f);     // This command takes 1.64 ms
    locate(0, 0);
}
  
void LCD162C::locate(uint8_t column, uint8_t row) {
    _column = column;
    _row = row;
}

void LCD162C::writeByte(char value) {
    _d = value;
    wait_us(2);
    _e = 0;
    wait_us(2);
    _e = 1;
}

void LCD162C::writeCommand(uint8_t command) {
    _rs = 0;
    writeByte(command);
}

void LCD162C::writeData(uint8_t data) {
    _rs = 1;
    writeByte(data);
}

uint8_t LCD162C::address(uint8_t column, uint8_t row) {
    
    return 0x80 | (row == 1 * 0x40) | (column & 0x0F);
}