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

void ethernetEchoExample(){
  EthernetInterface net;
  // Bring up the ethernet interface
  printf("Ethernet socket example\n");
  net.connect();

  // Show the network address
  const char *ip = net.get_ip_address();
  printf("IP address is: %s\n", ip ? ip : "No IP");

  // Open a socket on the network interface, and create a TCP connection to mbed.org
  TCPSocket socket;
  socket.open(&net);
  socket.connect(constants::ECHO_SERVER_ADDRESS, constants::ECHO_SERVER_PORT);

  // Send a simple http request
  char sbuffer[] = "GET / HTTP/1.1\r\nHost: www.arm.com\r\n\r\n";
  int scount = socket.send(sbuffer, sizeof sbuffer);
  printf("sent %d [%.*s]\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);


  // Recieve a simple http response and print out the response line
  char rbuffer[64];
  int rcount = socket.recv(rbuffer, sizeof rbuffer);
  pc.printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);
  // Close the socket to return its memory and bring down the network interface
  socket.close();
  // Bring down the ethernet interface
  net.disconnect();
  pc.printf("Done\n");
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
} coords;

Mail<coords, 10> communication;

void checker_thread() {
  static uint8_t change = 0;
  while (true){
    for(uint8_t y = 0; y < 8; y++) {
      change = mcps[y].getChanges(MCP23017_GPIO_PORT_B);  
      if(change){
        coords* pointer = communication.alloc();
        
        if(pointer == NULL){
          // not enough memory available
        } else {
          uint8_t x = 0 ;
          while (change){
            if(change&1) {
              pointer->x = x;
              pointer->y = y;
              communication.put(pointer);
            }
            x++;
            change >>=1;
          }
        }
      }
    }
  }
}


int main() 
{
  //TODO: Setup-Routine?
  // setup();
  i2c.frequency(100000);
  resetI2C(); 

  /**
  uint8_t a = 0;
  uint8_t x = 0;
  uint8_t change = 0;
  **/

  //TODO: Hier sinnvoll? Wo Verbindung und eth-int schließen?
  EthernetInterface net;
  net.connect();
  TCPSocket socket;
  socket.open(&net);
  socket.connect(constants::ECHO_SERVER_ADDRESS, constants::ECHO_SERVER_PORT);

  uint8_t status = constants::START;
  osEvent evt;
  coords buffer[3];
  //TODO: Aufräumen?!
  while (1)
  {
      switch(status) {
        case constants::START:
          evt = communication.get();
          if(evt.status == osEventMessage) {
            coords* nextCoord = (coords*) evt.value.p;
            buffer[0].x = nextCoord->x;
            buffer[0].y = nextCoord->y;
            //TODO: buffer[0].up = nextCoord->up;
            if(true) { //TODO: if(buffer[0].up) {

              status = constants::ONEUP;
            } else {
              //TODO: ERROR
            }
          }
          break;

        case constants::ONEUP:
          ledOn(buffer[0].x, buffer[0].y);
          evt = communication.get();
          if(evt.status == osEventMessage) {
            coords* nextCoord = (coords*) evt.value.p;
            buffer[1].x = nextCoord->x;
            buffer[1].y = nextCoord->y;
            //TODO: buffer[1].up = nextCoord->up;
            if(true) { //TODO: if(buffer[1].up) {
              
              status = constants::TWOUP;
            } else {
              if(buffer[0].x == buffer[1].x && buffer[0].y == buffer[1].y) {
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
          //TODO: Evtl. blinken?
          ledOn(buffer[1].x, buffer[1].y);
          evt = communication.get();
          if(evt.status == osEventMessage) {
            coords* nextCoord = (coords*) evt.value.p;
            buffer[2].x = nextCoord->x;
            buffer[2].y = nextCoord->y;
            //TODO: buffer[2].up = nextCoord->up;
            if(true) { //TODO: if(buffer[2].up) { 
              //TODO: ERROR
            } else {
              if(buffer[1].x == buffer[2].x && buffer[1].y == buffer[2].y) {
                //TODO: Abfrage nötig?
                status = constants::SEND;
              } else {
                //TODO: Error, oder nicht? s.o.
              }
            }
          }
          //TODO: TWOUP-Case
          break;

        case constants::SEND:
          //TODO: Buffer füllen und senden
          // protocol::TURN + x1 + y1 + x2 + y2
          //sbuffer = "4";
          //socket.send(sbuffer, sizeof sbuffer);
        
          status = constants::WAITING;
          //TODO: SEND-Case (Regulärer Move)
          break;
        
        case constants::WAITING:
          ledsOff();

          char rbuffer[64];
          int rcount = socket.recv(rbuffer, sizeof rbuffer);
          int offset = 0;
          if(!(rbuffer[0] & protocol::ERROR)) {
            if(!(rbuffer[0] & protocol::ILLEGAL)) {
              if(rbuffer[0] & protocol::CASTLING) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                ledOn(rbuffer[3 + offset], rbuffer[4 + offset]);
                //LCDO
                //TODO: Wait for 1 up and 2 down
                offset += 4;
              }

              if(rbuffer[0] & protocol::ENPASSANT) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                offset += 2;
                //LCDO
                //TODO: Wait for 1 up
              }

              if(rbuffer[0] & protocol::PROMOTION) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                offset += 2;

                //TODO: Promotion
              }

              if(rbuffer[0] & protocol::CHECK) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
                offset += 2;
              }

              if(rbuffer[0] & protocol::CHECKMATE) {
                ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);

                //TODO: End of game!
              }

              //TODO: Evtl. Delay vor dem AI-Move?

              ledsOff();
              
              if(rbuffer[1 + offset] & protocol::ILLEGAL) {
                ledOn(rbuffer[2 + offset], rbuffer[3 + offset]);
                ledOn(rbuffer[4 + offset], rbuffer[5 + offset]);
                offset += 4;

                //LCDO
                //TODO: Wait for 1 up and 2 down
              }

              if(rbuffer[1 + offset] & protocol::CASTLING) {
                ledOn(rbuffer[2 + offset], rbuffer[3 + offset]);
                ledOn(rbuffer[4 + offset], rbuffer[5 + offset]);
                ledOn(rbuffer[6 + offset], rbuffer[7 + offset]);
                ledOn(rbuffer[8 + offset], rbuffer[9 + offset]);
                offset += 8;
                
                //LCDO
                //TODO: Wait for 1 up and 2 down, 3 up and 4 down
              }

              if(rbuffer[1 + offset] & protocol::ENPASSANT) {
                ledOn(rbuffer[2 + offset], rbuffer[3 + offset]);
                ledOn(rbuffer[4 + offset], rbuffer[5 + offset]);
                ledOn(rbuffer[6 + offset], rbuffer[7 + offset]);
                offset += 6;
                
                //LCDO
                //TODO: Wait for 1 up and 2 down, 3 up
              }

              if(rbuffer[1 + offset] & protocol::PROMOTION) {
                ledOn(rbuffer[2 + offset], rbuffer[3 + offset]);
                ledOn(rbuffer[4 + offset], rbuffer[5 + offset]);
                offset += 4;

                //LCDO
                //TODO: Wait for 1 up and 2 down, 2 up and 2 down
              }

              if(rbuffer[1 + offset] & protocol::CHECK) {
                //TODO: An welcher Stelle steht die Position des Kings?
                ledOn(rbuffer[2 + offset], rbuffer[3 + offset]);
                ledOn(rbuffer[4 + offset], rbuffer[5 + offset]);
                ledOn(rbuffer[6 + offset], rbuffer[7 + offset]);

                //LCDO
                //TODO: Wait for 1 up and 2 down

              }

              if(rbuffer[1 + offset] & protocol::CHECKMATE) {
                //TODO: An welcher Stelle steht die Position des Kings?
                ledOn(rbuffer[2 + offset], rbuffer[3 + offset]);
                ledOn(rbuffer[4 + offset], rbuffer[5 + offset]);
                ledOn(rbuffer[6 + offset], rbuffer[7 + offset]);

                //LCDO
                //TODO: Wait for 1 up and 2 down
              }

            } else {
              ledOn(rbuffer[1 + offset], rbuffer[2 + offset]);
              ledOn(rbuffer[3 + offset], rbuffer[4 + offset]);
              //TODO: Wait for 1 up, 2 down
            }
          } else {
            //TODO: ERROR
          }

          status = constants::START;
          //TODO: WAITING-Case (Warten auf Serverantwort)
          break;
      }
    }


    
    /**
    change = 0;
    for(uint8_t y  = 0 ; y < 8 ; y++) {
      a = mcps[y].readGPIO(MCP23017_GPIO_PORT_B);
      change = mcps[y].getChanges(MCP23017_GPIO_PORT_B);  
      x = 0;
      while (change != 0) {
        if(change & 1){
          // manager.?(x,i)
        }  
        x++;
        change >>= 1;
      }

      errorCode = mcps[y].writeGPIO(MCP23017_GPIO_PORT_A, a ^ 0xff);
      if(errorCode != 0) {
        i2c.abort_transfer();
        pc.printf("%d\t%d\n", errorCode, y);
        errorCount++;
        printError(y);
        //mcps[i].reset();
        resetI2C();
      }
    }
    **/
}