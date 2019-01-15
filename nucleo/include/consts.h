#ifndef CONSTS_H
#define CONSTS_H

#include <mbed.h>

namespace constants {
    static const char* ECHO_SERVER_ADDRESS = "192.168.2.1";
    static const int ECHO_SERVER_PORT = 3000;

    static const char* OWN_ADDRESS = "192.168.2.2";
    static const char* NETMASK = "255.255.255.0";
    static const char* GATEWAY = "192.168.2.1";

    enum Status {
        FIRSTINIT = 0, NEWGAME = 90, START = 99, ONEUP = 1, TWOUP = 2, SEND = 4, WAITINGSERVER = 10, WAITINGPLAYER = 20, ERROR = 255
    };

    enum Player {
        WHITE = true, BLACK = false
    };

    static const char FIELDS[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    static const int TIMEOUT_GET_MAIL = 100;
    static const float TIMEOUT_BLINK = 0.2;

    enum UpDown {
        UP = true, DOWN = false
    };

    enum LedStatus {
        ON = 1, OFF = 0
    };

    static const PinName PIN_BUTTON_QUEEN = PC_6;
    static const PinName PIN_BUTTON_KNIGHT = PC_6;
    static const PinName PIN_BUTTON_BISHOP = PC_6; 
    static const PinName PIN_BUTTON_ROOK = PC_6;
    static const PinName PIN_BUTTON_START = PC_6;
    static const PinName PIN_BUTTON_AI = PC_6;
    static const PinName PIN_BUTTON_PVP = PB_15;
}



#endif 