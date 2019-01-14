#ifndef CONSTS_H
#define CONSTS_H
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

    enum UpDown {
        UP = true, DOWN = false
    };

    enum Buttons {
        BUTTON_QUEEN = PC_4, BUTTON_KNIGHT = PC_4, BUTTON_BISHOP = PC_4, BUTTON_ROOK = PC_4,
        BUTTON_START = PC_4, BUTTON_AI = PC_4, BUTTON_PVP = PC_4
    };
}



#endif 