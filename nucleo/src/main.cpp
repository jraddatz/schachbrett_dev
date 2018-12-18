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

//            rs, e   , d4  , d5  , d6  , d7
TextLCD lcd(PG_2, PG_3, PD_7, PD_6, PD_5, PD_4); 

MCP23017 mcps[8] = {
  MCP23017(0 , i2c ),MCP23017(1 , i2c ),
  MCP23017(2 , i2c ),MCP23017(3 , i2c ),
  MCP23017(4 , i2c ),MCP23017(5 , i2c ),
  MCP23017(6 , i2c ),MCP23017(7 , i2c )
};

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

void checker_thread(){
  static uint8_t change = 0;
  while (true){
    for(uint8_t y = 0; y < 8; y++){
      change = mcps[y].getChanges(MCP23017_GPIO_PORT_B);  
      if(change){
        coords* pointer = communication.alloc();
        
        if(pointer == NULL){
          // not enough memory available
        } else {
          uint8_t x = 0 ;
          while (change){
            if(change&1){
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
  //  setup();
  i2c.frequency(100000);
  resetI2C(); 
  uint8_t a = 0;
  uint8_t x = 0;
  uint8_t errorCode = 0;
  uint8_t change = 0;
  while (1)
  {
    change = 0;
    for(uint8_t y  = 0 ; y < 8 ; y++){
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
      if(errorCode != 0){
        i2c.abort_transfer();
        pc.printf("%d\t%d\n", errorCode, y);
        errorCount++;
        printError(y);
        //mcps[i].reset();
        resetI2C();
      }
    }
  }
}