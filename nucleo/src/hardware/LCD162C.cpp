
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
        writeByte(str[i++]);
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




// zweites Skript
#include "mbed.h"
//#include "KS0070B.h"
 

KS0070B::KS0070B(PinName _rs, PinName _rw, PinName _e,
PinName d0, PinName d1, PinName d2, PinName d3,
PinName d4, PinName d5, PinName d6, PinName d7)
: rs(_rs), rw(_rw), e(_e), databus(d0,d1,d2,d3,d4,d5,d6,d7)
{
    wait_ms(50);
    databus.input();
    rs=0;
    rw=1;
 
    send(0x20, true); // 4 bits
    send(0x28, true); // 4 bits, 2 lines, 5x7 dots
    send(0x0C, true); // display on, cursor on, blinking on
    send(0x01, true); // clear display
    send(0x06, true); // increase, not shifted
    send(0x02, true); // return home

}

KS0070B::KS0070B(PinName pin_e) : e(PB_6) , rw(PB_6), rs(PB_6), databus(PB_6)
{
    wait_ms(50);
    databus.input();
    rs=0;
    rw=1;
 
    send(0x20, true); // 4 bits
    send(0x28, true); // 4 bits, 2 lines, 5x7 dots
    send(0x0C, true); // display on, cursor on, blinking on
    send(0x01, true); // clear display
    send(0x06, true); // increase, not shifted
    send(0x02, true); // return home
}
 
void KS0070B::send(unsigned char data, bool isinstruction)
{
    bool busy=true;
    while(busy) {
        e=1;
        if(databus>>3==0) busy=false; // lcd is ready
        e=0;
        wait_us(4);
        e=1;
        wait_us(4);
        e=0;
        wait_us(4);
    }
    rs=!isinstruction;
    rw=0;
    databus.output();
 
    databus=data>>4; //MSB
    wait_us(1);
    e=1;
    wait_us(2);
    e=0;
    wait_us(2);
    databus=data; //LSB
    wait_us(1);
    e=1;
    wait_us(2);
    e=0;
    wait_us(2);
    databus.input();
 
    rs=0;
    rw=1;
}
 
KS0070B* KS0070B::print(const char *str)
{
    char temp;
    for(int i=0; i<100 && (temp=str[i])!='\0'; ++i) {
        if(temp<126) {
            if(temp!='%') {
                send(temp, false);
            } else {
                temp=str[++i];
                switch(temp) {
                    case 'd':
                        send(0xFD, false);
                        break;
                    case 'b':
                        send(0xFF, false);
                        break;
                    case 'l':
                        send(0x7E, false);
                        break;
                    case 'r':
                        send(0x7F, false);
                        break;
                    case 'm':
                        send(0xE4, false);
                        break;
                    case 'p':
                        send(0xF7, false);
                        break;
                    case 't':
                        send(0xF2, false);
                        break;
                    default:
                        send('%', false);
                        --i;
                        break;
                }
            }
        } else send(0xFF, false);
    }
    return this;
}
 
KS0070B* KS0070B::printnb(float nb, int left, int right) {
    char str[20];
    int p;
    if(left<0) left=0;
    sprintf(str, "%+f", nb);
    for(p=0; p<10 && str[p]!='.'; ++p);
    if(p>left+1) { // overflow
        str[1]='O';
        str[2]='F';
        for(int i=3; i<2+left+right; ++i) {
            str[i]=' ';
        }
    } else {
        p=1+left-p;
        for(int i=left+right+1; i>p; --i) {
            str[i]=str[i-p];
        }
        for(int i=p; i>0; --i) str[i]='0';
    }
    str[left+right+((right>0)?2:1)]='\0';
    print(str);
    
    return this;
}
 
KS0070B* KS0070B::clear() {
    send(0x01, true);
    send(0x02, true);
    return this;
}
 
KS0070B* KS0070B::pos(int line, int column) {
    if((line==0 || line==1) && (column>=0 && column<=15)) {
        send(0x80|(0x40*(line==1))|(column&0x0F), true);
    }
    return this;
}

void KS0070B::cls (){
    clear();
}
