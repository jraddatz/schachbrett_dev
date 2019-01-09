#ifndef CONSTS_H
#define CONSTS_H
namespace constants {
    static const char* ECHO_SERVER_ADDRESS = "169.254.73.121";
    static const int ECHO_SERVER_PORT = 3000;

    enum Status {
        INITBOARD = 0, START = 99, ONEUP = 1, TWOUP = 2, SEND = 4, WAITINGSERVER = 10, WAITINGPLAYER = 20
    };

    const char FIELDS[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    enum UpDown {
        UP = true, DOWN = false
    };
}



#endif 