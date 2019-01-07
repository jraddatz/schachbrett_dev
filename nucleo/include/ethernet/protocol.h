#ifndef ethernet_protocol_h
#define ethernet_protocol_h

#include <mbed.h>
#include <EthernetInterface.h>
#include "consts.h"

namespace protocol {
    enum Commands {
        START = 1, RESET = 2, TURN = 4, ENPASSANT = 4, PROMOTION = 8, CHECK = 16, CHECKMATE = 32, OK = 64, ERROR = 128
    };
    
    enum FigureChar {
        QUEEN = 'Q', KING = 'K', ROOK = 'R', BISHOP = 'B', KNIGHT = 'N', PAWN = 'P'
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