#ifndef ethernet_protocol_h
#define ethernet_protocol_h

#include <mbed.h>
#include <EthernetInterface.h>
#include "consts.h"

namespace protocol {
    enum Commands {
        start = 1, turn = 2
    };

    enum FigurChar {
        queen = 'Q', king = 'K', rock = 'R', bishop = 'B', knight = 'N', pawn = 'P'
    }; 

}

class ServerProtocol{
public:
    ServerProtocol(EthernetInterface &net);
    ~ServerProtocol();
    void connect();
    void startGame();
    void stopGame();
    void nextTurn(uint8_t, uint8_t, uint8_t, uint8_t, char);
private:
    TCPSocket socket;
    EthernetInterface& net;


};

#endif