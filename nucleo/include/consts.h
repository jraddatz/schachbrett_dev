#ifndef CONSTS_H
#define CONSTS_H

#include <mbed.h>

namespace constants {
    static const char* SERVER_ADDRESS = "192.168.2.1";
    static const int SERVER_PORT = 3000;

    static const char* OWN_ADDRESS = "192.168.2.2";
    static const char* NETMASK = "255.255.255.0";
    static const char* GATEWAY = "192.168.2.1";

    enum Status {
        FIRSTINIT = 0, NEWGAME = 10, BOARDSETUP = 20, START = 30, ONEUP = 40, TWOUP = 50, SEND = 60, WAITINGSERVER = 70, WAITINGPLAYER = 80, ENDGAME = 100, ERROR = 255
    };

    enum Player {
        WHITE = true, BLACK = false
    };

    static const char FIELDS[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    static const int TIMEOUT_GET_MAIL = 100;
    static const float TIMEOUT_BOARDSETUP = 0.5;
    static const float TIMEOUT_BLINK = 0.2;
    static const float TIMEOUT_WHILE_LOOP = 0.1;

    enum UpDown {
        UP = true, DOWN = false
    };

    enum LedStatus {
        ON = 1, OFF = 0
    };

    static const PinName PIN_BUTTON_QUEEN = PB_12;
    static const PinName PIN_BUTTON_KNIGHT = PA_15;
    static const PinName PIN_BUTTON_BISHOP = PC_7; 
    static const PinName PIN_BUTTON_ROOK = PB_5;
    static const PinName PIN_BUTTON_START = PB_13;
    static const PinName PIN_BUTTON_AI = PC_6;
    static const PinName PIN_BUTTON_PVP = PB_15;

    static const PinName PIN_I2C_DATA = PB_9;
    static const PinName PIN_I2C_CLOCK = PB_8;
    static const PinName PIN_I2C_NOTRESET = PG_1;

    // Pins für den LCD
    static const PinName PIN_LCD_RESET = PD_2;
    static const PinName PIN_LCD_RW = PG_2;
    static const PinName PIN_LCD_ENABLE = PG_3;
    static const PinName PIN_LCD_DATA_0 = PD_7;
    static const PinName PIN_LCD_DATA_1 = PD_6;
    static const PinName PIN_LCD_DATA_2 = PD_5;
    static const PinName PIN_LCD_DATA_3 = PD_4;
    static const PinName PIN_LCD_DATA_4 = PD_3;
    static const PinName PIN_LCD_DATA_5 = PE_2;
    static const PinName PIN_LCD_DATA_6 = PE_4;
    static const PinName PIN_LCD_DATA_7 = PE_5;
}



#endif 