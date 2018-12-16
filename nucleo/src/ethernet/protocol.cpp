#include "ethernet/protocol.h"
#include <mbed.h>
#include <EthernetInterface.h>

uint8_t convertTo(uint8_t x, uint8_t y)
{
    return x << 4 | y;
}

ServerProtocol::ServerProtocol(EthernetInterface &net) : net(net){}
 
ServerProtocol::~ServerProtocol()
{
    socket.close();
}

void ServerProtocol::connect()
{
    socket.open(&net);
    socket.connect(constants::ECHO_SERVER_ADDRESS, constants::ECHO_SERVER_PORT);
}

void ServerProtocol::startGame()
{
}

void ServerProtocol::stopGame()
{
}

void ServerProtocol::nextTurn(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, char newFigur)
{
    char buffer[4] = { protocol::turn,
                      convertTo(x1, y1),
                      convertTo(x2, y2),
                      newFigur };
}