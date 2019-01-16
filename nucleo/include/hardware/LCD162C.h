#ifndef HARDWARE_LCD162C_H
#define HARDWARE_LCD162C_H

#include "mbed.h"

class KS0070B;
 
class KS0070B
{
public:
    KS0070B(PinName pin_e);
    KS0070B* print(const char *str);
    KS0070B* printnb(float nb, int left, int right);
    KS0070B* clear();
    KS0070B* pos(int line, int column);    
    
private:
    void send(unsigned char data, bool isinstruction);
};
 


class LCD162C {
public:
    /**
     * @brief Creates A new TextLCD interface
     * 
     * @param rs The Reset 
     * @param rw Read/Write Selection Pin 
     * @param e  Enable line (clock)
     * @param d0 Data line 0
     * @param d1 Data line 1
     * @param d2 Data line 2
     * @param d3 Data line 3
     * @param d4 Data line 4
     * @param d5 Data line 5
     * @param d6 Data line 6
     * @param d7 Data line 7
     */
    LCD162C(PinName rs, PinName rw, PinName e,
    PinName d0, PinName d1, PinName d2, PinName d3,
    PinName d4, PinName d5, PinName d6, PinName d7);
    /**
     * @brief Simple print function to print a String on the LCD
     * 
     * @param string the String which will be shown on the LCD
     */
    void print(const char* string);

    /** Locate to a screen column and row
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     */
    void locate(uint8_t column, uint8_t row);

    /** Clear the screen and locate to 0,0 */
    void cls();

protected:

    uint8_t address(uint8_t column, uint8_t row);
    void writeByte(char value);
    void writeCommand(uint8_t command);
    void writeData(uint8_t data);

    DigitalOut _rs, _rw, _e;
    BusInOut _d;

    uint8_t _column;
    uint8_t _row;
};

#endif
