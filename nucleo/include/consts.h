#ifndef CONSTS_H
#define CONSTS_H
namespace constants {
    static const char* ECHO_SERVER_ADDRESS = "192.168.2.1";
    static const int ECHO_SERVER_PORT = 3000;

    static const char* OWN_ADDRESS = "192.168.2.2";
    static const char* NETMASK = "255.255.255.0";
    static const char* GATEWAY = "192.168.2.1";

    enum Status {
        INITBOARD = 0, START = 99, ONEUP = 1, TWOUP = 2, SEND = 4, WAITINGSERVER = 10, WAITINGPLAYER = 20
    };

    static const char FIELDS[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

    enum UpDown {
        UP = true, DOWN = false
    };
}



#endif 