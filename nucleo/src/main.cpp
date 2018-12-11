#include <mbed.h>
#include <EthernetInterface.h>
#include <SerialWireOutput.h>
#include "hardware/MCP23017.h"
#include "TextLCD.h"


const char* ECHO_SERVER_ADDRESS = "192.168.2.2";
const int ECHO_SERVER_PORT = 7;




Serial pc(USBTX, USBRX); // tx, rx
I2C i2c(PB_9, PB_8);
DigitalOut notReset ( PG_1 );
//DigitalOut pin (LED_1);
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
    socket.connect(ECHO_SERVER_ADDRESS, ECHO_SERVER_PORT);

    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: www.arm.com\r\n\r\n";
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);

    // Close the socket to return its memory and bring down the network interface
    socket.close();

    // Bring down the ethernet interface
    net.disconnect();
    printf("Done\n");

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

int main()
{
  EthernetInterface eth;
    // Bring up the ethernet interface
    printf("Ethernet socket example\n");
    net.connect();

    // Show the network address
    const char *ip = net.get_ip_address();
    printf("IP address is: %s\n", ip ? ip : "No IP");

    // Open a socket on the network interface, and create a TCP connection to mbed.org
    TCPSocket socket;
    socket.open(&net);
    socket.connect("www.arm.com", 80);

    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: www.arm.com\r\n\r\n";
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\n", scount, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);

    // Close the socket to return its memory and bring down the network interface
    socket.close();

    // Bring down the ethernet interface
    net.disconnect();
    printf("Done\n");
}