#ifndef GAME_H
#define GAME_H

#include <mbed.h>

#include <EthernetInterface.h>
#include "ethernet/protocol.h"
#include <SerialWireOutput.h>
#include "hardware/MCP23017.h"
#include "TextLCD.h"
#include "Board.h"
#include "consts.h"
#include "coords.h"

class Game {
public:
    /**
     * @brief Construct a new Game object
     * 
     */
    Game();

    /**
     * @brief Initialisation of the Game
     * 
     */
    void init();

    /**
     * @brief should be called to let the Game continue by one Step
     * 
     */
    void step();

private:
    /**
     * Checks if the sensors of all startfields are occupied
     * Turns the LED of the not occupied fields on
     * @return number of misplaced figures
     */
    uint8_t checkBoardSetup();

    /**
     * Thread for checking the GPIO Extenders. If there is a Sensor which did change, there will be a message written to the Communication Mail.
     */
    void checker_thread();

    /**
     * @brief Starts the Checker Thread
     * 
     */
    static void Checker_Thread_Start(void const *p);

    /**
     * Function to add a move that has to be done by the player to the pendingMoves Mail
     * @param x the x coordinate of the move
     * @param y the y coordinate of the move
     * @param up true for up, false for down
     */
    bool addPendingMove(uint8_t x, uint8_t y, bool up);

    /**
     * Clears the communication and pendingMoves Mails
     */
    void clearMails();

    /**
     * @brief 
     * 
     * @param socket 
     * @param commandByte 
     * @param byte1 
     * @param byte2 
     * @param byte3 
     * @param byte4 
     * @return uint8_t 
     */
    uint8_t sendTelegram(TCPSocket *socket, char commandByte, char byte1, char byte2, char byte3, char byte4);

    /**
     * @brief ISC incase the start Button was pressed
     * 
     */
    void startPressed();


    Board b;
    DigitalIn buttonQueen;
    DigitalIn buttonKnight;
    DigitalIn buttonBishop;
    DigitalIn buttonRook;
    DigitalIn buttonAI;
    DigitalIn buttonPVP;

    InterruptIn buttonStart;

    TCPSocket socket;

    TextLCD lcd;

    EthernetInterface net;


    /**
     * @brief Current Game Status
     * 
     */
    uint8_t status;
    /**
     * @brief All Actions which got read from the Sensors
     * 
     */
    Mail<coords, 10> communication;
    osEvent evtCommunication;
    /**
     * @brief All Moves the Player has tu do until a new Move is allowed
     * contains up to 10 wring Moves
     * 
     */
    Mail<coords, 10> pendingMoves;
    osEvent evtPendingMoves;
    // Players move
    coords bufferPlayerMoves[3];
    // Network Buffer
    int offset;
    char rbuffer[64];
    int rcount;

    bool buttonPressed;
    int gameType;
    bool player;
    bool isPromoted = false;
    coords checkmate;
    /**
     * @brief 
     * 
     */
    uint8_t gameEnded;
    // Checker Thread
    Thread thread;

};

#endif