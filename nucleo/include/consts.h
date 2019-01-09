#ifndef CONSTS_H
#define CONSTS_H
namespace constants {
    static const char* ECHO_SERVER_ADDRESS = "1.1.1.1";
    static const int ECHO_SERVER_PORT = 7;

    enum Status {
        START = 0, ONEUP = 1, TWOUP = 2, SEND = 4, WAITINGSERVER = 10, WAITINGPLAYER = 20
    };

    const char FIELDS[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    enum UpDown {
        UP = true, DOWN = false
    };
}



#endif 