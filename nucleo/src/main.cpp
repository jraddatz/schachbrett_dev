#include <mbed.h>
#include <EthernetInterface.h>
#include "ethernet/protocol.h"
#include <SerialWireOutput.h>
#include "hardware/MCP23017.h"
#include "TextLCD.h"
#include "consts.h"

Serial pc(USBTX, USBRX); // tx, rx
I2C i2c(PB_9, PB_8);
DigitalOut notReset ( PG_1 );
uint8_t errorCode = 0;

TextLCD lcd(PG_2, PG_3, PD_7, PD_6, PD_5, PD_4); 

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

void ledOn(uint8_t x, uint8_t y){
  if(x >= 8 || y >= 8){
    return ;
  }

  uint8_t a = mcps[y].readGPIO(MCP23017_GPIO_PORT_A);
  mcps[y].writeGPIO(MCP23017_GPIO_PORT_A, a | 1 << x );
}

void ledsOff(){
  for(uint8_t y = 0; y < 8; y++){
    errorCode = mcps[y].writeGPIO(MCP23017_GPIO_PORT_A, 0x00);
    if (errorCode) {
      // reset();
    }
  }
}


void scanI2c(I2C &ic)
{
  char pointer[1] = {0};
  for (int i = 0; i < 256; i++)
  {
    if (ic.write(i, pointer, 0) == 0)
    {
      pc.printf("Found at Address: %d\n", i);
    }
    else
    {
      pc.printf("Found nothing at Address: %d\n", i);
    }
  }
}

void resetI2C(){
  notReset = 0;
  wait_ms(1);
  notReset = 1;
  for(int i = 0; i < 8 ; i++){
    mcps[i].init();
  }

}

uint16_t errorCount = 0;

void printError (uint8_t mcpID){
  lcd.cls();
  lcd.printf("%d\n", errorCount);
  lcd.printf("%d\n", mcpID);
}

typedef struct {
  uint8_t x;
  uint8_t y;
  bool up;
} coords;

Mail<coords, 10> communication;

void checker_thread() {
  static uint8_t change = 0;
  while (true){
    for(uint8_t y = 0; y < 8; y++) {
      change = mcps[y].getChanges(MCP23017_GPIO_PORT_B);  
      if(change){
        printf("Checker");
        coords* pointer = communication.alloc();
        
        if(pointer == NULL){
          // not enough memory available
        } else {
          uint8_t a = mcps[y].readGPIO(MCP23017_GPIO_PORT_B);
          uint8_t x = 0 ;
          while (change) {
            if(change&1) {
              pointer->x = x;
              pointer->y = y;
              pointer->up = (a & 1 << x) != 0;
              communication.put(pointer);
              printf("x = %d; y = %d; down = %u\n", x, y, (a >> x & 1));
            }
            x++;
            change >>=1;
          }
        }
      }
    }
    wait(0.1);
  }
}

Mail<coords, 10> pendingMoves;

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

char sendBuffer[5];
int offset;
char rbuffer[64];
int rcount;
Thread thread;

int main() 
{
  //TODO: Setup-Routine?
  // setup();
  i2c.frequency(100000);
  resetI2C(); 

  EthernetInterface net;
  net.connect();
  TCPSocket socket;

  uint8_t status = constants::INITBOARD;
  osEvent evtCommunication;
  osEvent evtPendingMoves;
  coords bufferPlayerMoves[3];
  //TODO: Aufräumen?!
  while (1)
  {
      switch(status) {
        case constants::INITBOARD:
          for(uint8_t y = 0; y < 8; y++) {
            mcps[y].getChanges(MCP23017_GPIO_PORT_B); 
          }
          //TODO: Thread nur einmal starten
          //if(thread.get_state() != Thread::Running) {
            thread.start(checker_thread);
          //}
          status = constants::START;
          break; 
        case constants::START:
          printf("Start\n");
          evtCommunication = communication.get();
          printf("StartAfterGet\n");
          if(evtCommunication.status == osEventMail) {
            printf("StartAfterChange\n");
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
              lcd.printf("Bitte Figur anheben.");
              status = constants::WAITINGPLAYER;
            }
          }
          break;

        case constants::ONEUP:
          printf("Oneup\n");
          ledOn(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y);
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
          //TODO: Evtl. blinken?
          ledOn(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y);
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
              lcd.printf("3 Figuren anheben nicht möglich!");
              status = constants::WAITINGPLAYER;
            } else {
              if(bufferPlayerMoves[1].x == bufferPlayerMoves[2].x && bufferPlayerMoves[1].y == bufferPlayerMoves[2].y) {
                status = constants::SEND;
              } else {
                addPendingMove(bufferPlayerMoves[2].x, bufferPlayerMoves[2].y, !bufferPlayerMoves[2].up);
                addPendingMove(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y, !bufferPlayerMoves[1].up);
                addPendingMove(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y, !bufferPlayerMoves[0].up);

                lcd.cls();
                lcd.printf("Figur muss an gleiche Stelle wie Geschlagene");
                status = constants::WAITINGPLAYER;
              }
            }
          }
          //TODO: TWOUP-Case
          break;

        case constants::SEND:
          printf("Send\n");
          socket.open(&net);
          socket.connect(constants::ECHO_SERVER_ADDRESS, constants::ECHO_SERVER_PORT);

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
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                ledOn(rbuffer[3 + offset], rbuffer[4 + offset]);
                lcd.cls();
                lcd.printf("Rochade: Turm setzen");

                printf("Player: Castling\n");


                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);
                offset += 4;
              }

              if(rbuffer[0] & protocol::ENPASSANT) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);

                printf("Player: Enpassant\n");

                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                offset += 2;
                lcd.cls();
                lcd.printf("En passante");
                
              }

              if(rbuffer[0] & protocol::PROMOTION) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                offset += 2;

                //TODO: Promotion
              }

              if(rbuffer[0] & protocol::CHECK) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);

                printf("Player: Check\n");

                offset += 2;
              }

              if(rbuffer[0] & protocol::CHECKMATE) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);

                printf("Player: Checkmate\n");


                //TODO: End of game!
              }

              //TODO: Evtl. Delay vor dem AI-Move?

              ledsOff();
              
              // ***************************************************
              // Ab hier AI-Move
              // ***************************************************

              offset = protocol::AI_MOVE;

              if(rbuffer[0 + protocol::AI_MOVE] & protocol::ILLEGAL) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                ledOn(rbuffer[3 + offset], rbuffer[4 + offset]);

                printf("AI OK\n");

                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);

                offset += 4;

                lcd.cls();
                lcd.printf("KI: Normaler Move");
                
              }

              if(rbuffer[0 + protocol::AI_MOVE] & protocol::CASTLING) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                ledOn(rbuffer[3 + offset], rbuffer[4 + offset]);
                ledOn(rbuffer[5 + offset], rbuffer[6 + offset]);
                ledOn(rbuffer[7 + offset], rbuffer[8 + offset]);

                printf("AI Castling\n");

                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);
                addPendingMove(rbuffer[5 + offset], rbuffer[6 + offset], constants::UP);
                addPendingMove(rbuffer[7 + offset], rbuffer[8 + offset], constants::DOWN);

                offset += 8;
                
                lcd.cls();
                lcd.printf("KI: Rochade");
              }

              if(rbuffer[0 + protocol::AI_MOVE] & protocol::ENPASSANT) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                ledOn(rbuffer[3 + offset], rbuffer[4 + offset]);
                ledOn(rbuffer[5 + offset], rbuffer[6 + offset]);

                printf("AI Enpassant\n");

                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);
                addPendingMove(rbuffer[5 + offset], rbuffer[6 + offset], constants::UP);

                offset += 6;
                
                lcd.cls();
                lcd.printf("KI: En passante");
              }

              if(rbuffer[0 + protocol::AI_MOVE] & protocol::PROMOTION) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                ledOn(rbuffer[3 + offset], rbuffer[4 + offset]);

                printf("AI Promotion\n");

                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);
                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::UP);
                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);                
                
                offset += 4;                
                
                lcd.cls();
                lcd.printf("KI: Promotion");

                //TODO: Figur zu der befördert wird anzeigen
              }

              if(rbuffer[0 + protocol::AI_MOVE] & protocol::CHECK) {
                ledOn(rbuffer[5 + offset], rbuffer[6 + offset]);
                
                printf("AI Check\n");

                //LCDO
                //TODO: Vorher Delay?
              }

              if(rbuffer[0 + protocol::AI_MOVE] & protocol::CHECKMATE) {
                ledOn(rbuffer[5 + offset], rbuffer[6 + offset]);

                printf("AI Checkmate\n");

                //LCDO
                //TODO: Vorher Delay?
                //TODO: End of Game
              }

            } else {
              //Illegal Playermove

              printf("Player Illegal\n");

              ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
              ledOn(rbuffer[3 + offset], rbuffer[4 + offset]);

              addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
              addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);

              lcd.cls();
              lcd.printf("Illegaler Move");
            }
          } else {
            //TODO: ERROR - Fehlerbit vom Server gesetzt
          }

          status = constants::WAITINGPLAYER;
          //TODO: WAITINGSERVER-Case (Warten auf Serverantwort)
          break;

        case constants::WAITINGPLAYER:
          printf("Waitingplayer\n");
          evtPendingMoves = pendingMoves.get();
          while(evtPendingMoves.status == osEventMail) {
            coords* nextPending = (coords*) evtPendingMoves.value.p;
              bool moveMade = false;
              while(!moveMade) {
                evtCommunication = communication.get();
                if(evtPendingMoves.status == osEventMail) {
                  coords* nextMade = (coords*) evtPendingMoves.value.p;
                  if(!(nextPending->x == nextMade->x && nextPending->y == nextMade->y && nextPending->up == nextMade->up)) {
                    //TODO: ERROR - Spieler hat falschen Move gemacht
                  }
                  moveMade = true;

                  pendingMoves.free(nextPending);
                  communication.free(nextMade);
                }
              }
            evtPendingMoves = pendingMoves.get();
          }

          status = constants::START;
        break;
      }
    }
}