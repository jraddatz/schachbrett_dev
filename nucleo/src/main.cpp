#include <mbed.h>
#include <EthernetInterface.h>
#include "ethernet/protocol.h"
#include <SerialWireOutput.h>
#include "hardware/MCP23017.h"
#include "TextLCD.h"
#include "consts.h"

Serial pc(USBTX, USBRX); // tx, rx
I2C i2c(constants::PIN_I2C_DATA, constants::PIN_I2C_CLOCK);
DigitalOut notReset (constants::PIN_I2C_NOTRESET);
uint8_t errorCode = 0;
//          rs,   e   , D4  , D5  , D6  , D7
TextLCD lcd(constants::PIN_LCD_RESET, constants::PIN_LCD_ENABLE, constants::PIN_LCD_DATA_4, 
constants::PIN_LCD_DATA_5, constants::PIN_LCD_DATA_6, constants::PIN_LCD_DATA_7); 

DigitalIn buttonQueen(constants::PIN_BUTTON_QUEEN, PullUp);
DigitalIn buttonKnight(constants::PIN_BUTTON_KNIGHT, PullUp);
DigitalIn buttonBishop(constants::PIN_BUTTON_BISHOP, PullUp);
DigitalIn buttonRook(constants::PIN_BUTTON_ROOK, PullUp);
InterruptIn buttonStart(constants::PIN_BUTTON_START);
DigitalIn buttonAI(constants::PIN_BUTTON_AI, PullUp);
DigitalIn buttonPVP(constants::PIN_BUTTON_PVP, PullUp);

void startPressed() {

}

//TODO: Send-Methode auslagern

MCP23017 mcps[8] = {
  MCP23017(0 , i2c ),MCP23017(1 , i2c ),
  MCP23017(2 , i2c ),MCP23017(3 , i2c ),
  MCP23017(4 , i2c ),MCP23017(5 , i2c ),
  MCP23017(6 , i2c ),MCP23017(7 , i2c )
};

/**
 * TODO: Funktion um x,y Koordinaten in String umzuformen
char[] stringifyCoords(uint8_t x, uint8_t y) {
  char retCoords[] = constants::FIELDS[x] + (y + 1);
  return retCoords;
}
**/

/**
 * 
 */
void setup(){
  
}

/**
 * toggle an LED at the given coordinates
 * @param x the x coordinate of the LED
 * @param y the y coordinate of the LED
 * @oaram on 1 for on, 0 for off (default is 1)
 *    
 */
void ledToggle(uint8_t x, uint8_t y, uint8_t on = 1 ){
  if(x >= 8 || y >= 8){
    return ;
  }
  uint8_t a = mcps[x].readGPIO(MCP23017_GPIO_PORT_A);
  a = on ? a | 1 << y : a & ~(1<<y);
  mcps[x].writeGPIO(MCP23017_GPIO_PORT_A, a );
}

/**
 * returns the status of the field at given coordinates
 * @param x the x coordinate of the field
 * @param y the y coordinate of the field
 * @return 1 for occupied, 0 for empty
 */
uint8_t checkField (uint8_t x, uint8_t y){
  if(x >= 8 || y >= 8){
    return -1; // error
  }
  uint8_t a = mcps[x].readGPIO(MCP23017_GPIO_PORT_B);
  return (a >> y & 1) ^ 1;
}

/**
 * Toggles all LEDs off
 */
void ledsOff(){
  for(uint8_t x = 0; x < 8; x++){
    errorCode = mcps[x].writeGPIO(MCP23017_GPIO_PORT_A, 0x00);
    if (errorCode) {
      // reset();
    }
  }
}

/**
 * Checks if the sensors of all startfields are occupied
 * Turns the LED of the not occupied fields on
 * @return number of misplaced figures
 */
uint8_t checkBoardSetup(){
  ledsOff();
  uint8_t misplaced = 0;
  for ( uint8_t x = 0 ; x < 8; x++) {
    for (uint8_t y = 0; y < 2; y++) {
      if(!checkField(x,y)) {
        ledToggle(x,y);
        misplaced++;
      }
    }
    for (uint8_t y = 6; y < 8; y++) {
      if(!checkField(x,y)){
        ledToggle(x,y);
        misplaced++;
      }
    }
  }
  return misplaced;
}

/**
 * Helper Function to reset all GPIO Extender
 */
void resetI2C(){
  notReset = 0;
  wait_ms(1);
  notReset = 1;
  for(int i = 0; i < 8 ; i++){
    mcps[i].init();
  }

}

/**
 * struct for the move of a single figure
 */
typedef struct {
  uint8_t x;
  uint8_t y;
  bool up;
} coords;

Mail<coords, 10> communication;
/**
 * Thread for checking the GPIO Extenders. If there is a Sensor which did change, there will be a message written to the Communication Mail.
 */
void checker_thread() {
  static uint8_t change = 0;
  while (true){
    for(uint8_t x = 0; x < 8; x++) {
      change = mcps[x].getChanges(MCP23017_GPIO_PORT_B);  
      if(change){
        coords* pointer = communication.alloc();
        
        if(pointer == NULL){
          // not enough memory available maybe there is a error ?
        } else {
          uint8_t a = mcps[x].readGPIO(MCP23017_GPIO_PORT_B);
          uint8_t y = 0 ;
          while (change) {
            if(change&1) {
              pointer->x = x;
              pointer->y = y;
              pointer->up = (a & 1 << y) != 0;
              communication.put(pointer);
              printf("x = %d; y = %d; down = %u\n", x, y, (a >> y & 1));
            }
            y++;
            change >>=1;
          }
        }
      }
    }
    wait(constants::TIMEOUT_WHILE_LOOP); 
  }
}

Mail<coords, 10> pendingMoves;

//TODO: bool in uint umstellen?
/**
 * Function to add a move that has to be done by the player to the pendingMoves Mail
 * @param x the x coordinate of the move
 * @param y the y coordinate of the move
 * @param up true for up, false for down
 */
bool addPendingMove(uint8_t x, uint8_t y, bool up) {
  coords* pointer = pendingMoves.alloc();
  
  if(pointer == NULL){
    // not enough memory available
    return false;
  } else {
    pointer->x = x;
    pointer->y = y;
    pointer->up = up;
    pendingMoves.put(pointer);
  }
  return true;
}

/**
 * Clears the communication and pendingMoves Mails
 */
void clearMails() {
  osEvent evtCommunication = communication.get(constants::TIMEOUT_GET_MAIL);
  while(evtCommunication.status == osEventMail) {            
    coords* nextCoord = (coords*) evtCommunication.value.p;
    communication.free(nextCoord);
    evtCommunication = communication.get(constants::TIMEOUT_GET_MAIL);
  }

  osEvent evtPendingMoves = pendingMoves.get(constants::TIMEOUT_GET_MAIL);
  while(evtPendingMoves.status == osEventMail) {            
    coords* nextCoord = (coords*) evtPendingMoves.value.p;
    pendingMoves.free(nextCoord);
    evtPendingMoves = pendingMoves.get(constants::TIMEOUT_GET_MAIL);
  }
}

EthernetInterface net;

uint8_t sendTelegram(TCPSocket* socket, char commandByte, char byte1, char byte2, char byte3, char byte4) {
  if (socket->open(&net) != 0) {
    return -1;
  } else {
    printf("Socket open\n");
    if (socket->connect(constants::SERVER_ADDRESS, constants::SERVER_PORT) != 0) {
      return -1;
    } else {
      printf("Socket connected\n");
      char sendBuffer[5] = {commandByte, byte1, byte2, byte3, byte4};
      if(socket->send(sendBuffer, sizeof sendBuffer) != 0) {
        return 1; 
      }
    }
  }
}

char sendBuffer[5];
int offset;
char rbuffer[64];
int rcount;
Thread thread;
bool isPromoted = false;

int main() 
{
  //TODO: Setup-Routine?
  // setup();
  i2c.frequency(100000);
  resetI2C(); 

  buttonStart.rise(&startPressed);

  //net.set_network(constants::OWN_ADDRESS, constants::NETMASK, constants::GATEWAY);
  net.connect();

  TCPSocket socket;

  uint8_t status = constants::NEWGAME;
  osEvent evtCommunication;
  osEvent evtPendingMoves;
  coords bufferPlayerMoves[3];
  bool buttonPressed;
  int gameType;
  bool player;
  coords checkmate;
  uint8_t gameEnded;

  for(uint8_t y = 0; y < 8; y++) {
    mcps[y].getChanges(MCP23017_GPIO_PORT_B); 
  }
  thread.start(checker_thread);

  while (1)
  {
      switch(status) {
        case constants::NEWGAME:
          printf("NEWGAME\n");

          gameEnded = 0;
          player = constants::WHITE;

          lcd.cls();
          lcd.printf("Choose Mode!");

          buttonPressed = false;
          while (!buttonPressed) {
            if(!buttonAI.read()) {
              gameType = protocol::AI;
              buttonPressed = true;
            } else if (!buttonPVP.read()) {
              gameType = protocol::PVP;
              buttonPressed = true;
            }
          }

         
          if(sendTelegram(&socket, protocol::START, gameType, 0, 0, 0) == 1) {
            if(socket.recv(rbuffer, sizeof rbuffer) < 0) {
              status = constants::ERROR;
            } else {
              printf("Socket recvd\n");
              if(rbuffer[0] == 0) {
                status = constants::BOARDSETUP;
              }  else {
                status = constants::ERROR;
              }
            }  
            socket.close(); 
            printf("Socket closed\n");
          } else {
            status = constants::ERROR;
          }
          break;

        case constants::BOARDSETUP:
          if(checkBoardSetup() == 0) {
            clearMails();
            status = constants::START;
          }

          wait(constants::TIMEOUT_BOARDSETUP);
          break;

        case constants::START:
          printf("Start\n");
          evtCommunication = communication.get();
          if(evtCommunication.status == osEventMail) {
            coords* nextCoord = (coords*) evtCommunication.value.p;
            bufferPlayerMoves[0].x = nextCoord->x;
            bufferPlayerMoves[0].y = nextCoord->y;
            bufferPlayerMoves[0].up = nextCoord->up;
            communication.free(nextCoord);
            if(bufferPlayerMoves[0].up) {
              status = constants::ONEUP;
            } else {
              addPendingMove(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y, !bufferPlayerMoves[0].up);
              lcd.cls();
              lcd.printf("Bitte Figur anheben.\n");
              status = constants::WAITINGPLAYER;
            }
          }
          break;

        case constants::ONEUP:
          printf("Oneup\n");
          ledToggle(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y);
          evtCommunication = communication.get();
          if(evtCommunication.status == osEventMail) {
            coords* nextCoord = (coords*) evtCommunication.value.p;
            bufferPlayerMoves[1].x = nextCoord->x;
            bufferPlayerMoves[1].y = nextCoord->y;
            bufferPlayerMoves[1].up = nextCoord->up;
            communication.free(nextCoord);
            if(bufferPlayerMoves[1].up) {
              
              status = constants::TWOUP;
            } else {
              if(bufferPlayerMoves[0].x == bufferPlayerMoves[1].x && bufferPlayerMoves[0].y == bufferPlayerMoves[1].y) {
                ledsOff();
                status = constants::START;
              } else {
                status = constants::SEND;
              }
            }
          }
          //TODO: ONEUP-Case
          break;

        case constants::TWOUP:
          printf("Twoup\n");
          ledToggle(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y);
          evtCommunication = communication.get();
          if(evtCommunication.status == osEventMail) {
            coords* nextCoord = (coords*) evtCommunication.value.p;
            bufferPlayerMoves[2].x = nextCoord->x;
            bufferPlayerMoves[2].y = nextCoord->y;
            bufferPlayerMoves[2].up = nextCoord->up;
            communication.free(nextCoord);
            if(bufferPlayerMoves[2].up) { 
              addPendingMove(bufferPlayerMoves[2].x, bufferPlayerMoves[2].y, !bufferPlayerMoves[2].up);
              addPendingMove(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y, !bufferPlayerMoves[1].up);              
              addPendingMove(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y, !bufferPlayerMoves[0].up);

              lcd.cls();
              lcd.printf("3 Figuren anheben nicht möglich!\n");
              status = constants::WAITINGPLAYER;
            } else {
              //TODO: Zwei Moves/Coords vergleichen als Hilfsfunktion
              if(bufferPlayerMoves[1].x == bufferPlayerMoves[2].x && bufferPlayerMoves[1].y == bufferPlayerMoves[2].y) {
                status = constants::SEND;
                ledToggle(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y, constants::OFF);
                wait(constants::TIMEOUT_BLINK);
              } else {
                addPendingMove(bufferPlayerMoves[2].x, bufferPlayerMoves[2].y, !bufferPlayerMoves[2].up);
                addPendingMove(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y, !bufferPlayerMoves[1].up);
                addPendingMove(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y, !bufferPlayerMoves[0].up);

                lcd.cls();
                lcd.printf("Figur muss an gleiche Stelle wie Geschlagene\n");
                status = constants::WAITINGPLAYER;
              }
            }
          }
          //TODO: TWOUP-Case
          break;

        case constants::SEND:
          printf("Send\n");

          ledToggle(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y);

          socket.open(&net);
          socket.connect(constants::SERVER_ADDRESS, constants::SERVER_PORT);

          sendBuffer[0] = protocol::TURN;
          sendBuffer[1] = bufferPlayerMoves[0].x;
          sendBuffer[2] = bufferPlayerMoves[0].y; 
          sendBuffer[3] = bufferPlayerMoves[1].x;
          sendBuffer[4] = bufferPlayerMoves[1].y;

          socket.send(sendBuffer, sizeof sendBuffer);
        
          status = constants::WAITINGSERVER;
          break;
        
        case constants::WAITINGSERVER:
          printf("Waitingserver\n");
          ledsOff();

          rcount = socket.recv(rbuffer, sizeof rbuffer);
          socket.close();

          offset = 0;
          if(!(rbuffer[0] & protocol::ERROR)) {
            if(!(rbuffer[0] & protocol::ILLEGAL)) {
              if(rbuffer[0] & protocol::CASTLING) {
                ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);
                ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);
                lcd.cls();
                lcd.printf("Rochade: Turm setzen\n");

                printf("Player: Castling\n");


                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);
                offset += 4;
              }

              if(rbuffer[0] & protocol::ENPASSANT) {
                ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);

                printf("Player: Enpassant\n");

                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                offset += 2;

                lcd.cls();
                lcd.printf("En passante\n");
                
              }

              if(rbuffer[0] & protocol::PROMOTION) {
                isPromoted = true;
                sendBuffer[0] = protocol::PROMOTION;

                ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);
                while ( (buttonQueen.read() && buttonKnight.read() && buttonBishop.read() && buttonRook.read() )) {
                  wait(0.1);
                }

                if(!buttonQueen.read()) {
                  sendBuffer[1] = protocol::QUEEN;
                } else if (!buttonKnight.read()) {
                  sendBuffer[1] = protocol::KNIGHT;
                } else if (!buttonBishop.read()) {
                  sendBuffer[1] = protocol::BISHOP;
                } else if (!buttonRook.read()) {
                  sendBuffer[1] = protocol::ROOK;
                }

                socket.open(&net);
                socket.connect(constants::SERVER_ADDRESS, constants::SERVER_PORT);
                socket.send(sendBuffer, sizeof sendBuffer);

                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::DOWN);

                offset += 2;

                //LCDO
                //TODO: Promotion
              }

              if(rbuffer[0] & protocol::CHECK) {
                ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);

                printf("Player: Check\n");

                offset += 2;
              }

              if(rbuffer[0] & protocol::CHECKMATE) {
                  ledsOff();

                printf("Player: Checkmate\n");

                checkmate.x = rbuffer[3 + offset];
                checkmate.y = rbuffer[4 + offset];

                gameEnded = 1;
              }

              //TODO: Evtl. Delay vor dem AI-Move?

              ledsOff();
              if(gameType == protocol::AI) {

                // ***************************************************
                // Ab hier AI-Move
                // ***************************************************

                offset = protocol::AI_MOVE;

                if(rbuffer[0 + protocol::AI_MOVE] & protocol::ILLEGAL) {
                  ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);
                  ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);

                  printf("AI OK\n");

                  
                  addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                  if(checkField(rbuffer[3 + offset], rbuffer[4 + offset])) addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::UP);
                  addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);

                  printf("Erwarte Move\nx=%d, y=%d, UP\nx=%d, y=%d, DOWN", rbuffer[1 + offset], rbuffer[2 + offset], rbuffer[3 + offset], rbuffer[4 + offset]);

                  offset += 2;

                  lcd.cls();
                  lcd.printf("KI: Normaler Move\n");
                  
                if(rbuffer[0 + protocol::AI_MOVE] & protocol::CASTLING) {
                  ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);
                  ledToggle(rbuffer[5 + offset], rbuffer[6 + offset]);

                  printf("AI Castling\n");

                  addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::UP);
                  addPendingMove(rbuffer[5 + offset], rbuffer[6 + offset], constants::DOWN);

                  offset += 6;
                  
                  lcd.cls();
                  lcd.printf("KI: Rochade\n");
                }

                if(rbuffer[0 + protocol::AI_MOVE] & protocol::ENPASSANT) {
                    ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);

                    printf("AI Enpassant\n");

                    addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::UP);

                    offset += 4;
                    
                    lcd.cls();
                    lcd.printf("KI: En passante\n");
                  }

                  if(rbuffer[0 + protocol::AI_MOVE] & protocol::PROMOTION) {
                    printf("AI Promotion\n");

                    addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                    addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::DOWN);                
                    
                    offset += 6;                
                    
                    lcd.cls();
                    lcd.printf("KI: Promotion\n");
                    //LCDO
                    //TODO: Figur zu der befördert wird anzeigen
                  }

                }



                if(rbuffer[0 + protocol::AI_MOVE] & protocol::CHECK) {
                  ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);
                  
                  printf("AI Check\n");

                  //LCDO
                  //TODO: Vorher Delay?
                }

                if(rbuffer[0 + protocol::AI_MOVE] & protocol::CHECKMATE) {
                  ledsOff();

                  printf("AI Checkmate\n");

                  checkmate.x = rbuffer[3 + offset];
                  checkmate.y = rbuffer[4 + offset];

                  gameEnded = 1;                  
                  //LCDO
                  //TODO: Vorher Delay?
                  //TODO: End of Game
                }

              }

            if(!isPromoted && gameType == protocol::PVP) {
              player = !player;
            }

            } else {
              printf("Player Illegal\n");

              ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);
              ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);
              
              addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
              addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);
              if(bufferPlayerMoves[2].x == bufferPlayerMoves[1].x && bufferPlayerMoves[1].y == bufferPlayerMoves[2].y && bufferPlayerMoves[1].up != bufferPlayerMoves[2].up) {
                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::DOWN);
              }

              lcd.cls();
              lcd.printf("Illegaler Move\n");
            }
          } else {
            status = constants::ERROR;
          }

          if(status != constants::ENDGAME) status = constants::WAITINGPLAYER;
          //TODO: WAITINGSERVER-Case (Warten auf Serverantwort)
          break;

        case constants::WAITINGPLAYER:
          printf("Waitingplayer\n");
          evtPendingMoves = pendingMoves.get(constants::TIMEOUT_GET_MAIL);
          while(evtPendingMoves.status == osEventMail) {
            coords* nextPending = (coords*) evtPendingMoves.value.p;
            ledToggle(nextPending->x, nextPending->y, constants::ON);
            bool moveMade = false;
            while(!moveMade) {
              evtCommunication = communication.get(constants::TIMEOUT_GET_MAIL);
              if(evtCommunication.status == osEventMail) {
                coords* nextMade = (coords*) evtCommunication.value.p;
                if((nextPending->x == nextMade->x && nextPending->y == nextMade->y && nextPending->up == nextMade->up)) {
                  ledToggle(nextPending->x, nextPending->y, constants::OFF);
                  moveMade = true;
                  wait(constants::TIMEOUT_BLINK);
                } else {      
                  //printf("Pending != nextMade\n");
                  //TODO: ERROR - Spieler hat falschen Move gemacht            
                } 
                communication.free(nextMade);
              }
            }
            pendingMoves.free(nextPending);
            evtPendingMoves = pendingMoves.get(constants::TIMEOUT_GET_MAIL);
          }

          if(gameEnded) {
            status = constants::ENDGAME;
          } else {
            if(isPromoted) {
              status = constants::WAITINGSERVER;
              isPromoted = false;
            } else {
              status = constants::START;
            }
          }
        break;

        case constants::ENDGAME:
          printf("ENDGAME\n");
          ledToggle(checkmate.x, checkmate.y, gameEnded);
          gameEnded ^= 1;
          wait(constants::TIMEOUT_BLINK);
          break;

        case constants::ERROR:
          printf("----\nError\n-----");
          break;
      }
      wait(constants::TIMEOUT_WHILE_LOOP);
    }
}