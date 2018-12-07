#include <mbed.h>
#include <SerialWireOutput.h>
#include "hardware/MCP23017.h"
#include "TextLCD.h"


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
  i2c.frequency(100000);
  resetI2C(); 
  uint8_t a = 0;
  uint8_t errorCode = 0;
  while (1)
  {
    wait_ms(1);
    for(uint8_t i  = 0 ; i < 8 ; i++){
      a = mcps[i].readGPIO(MCP23017_GPIO_PORT_B);
      
      errorCode = mcps[i].writeGPIO(MCP23017_GPIO_PORT_A, a ^ 0xff);
      if(errorCode != 0){
        i2c.abort_transfer();
        pc.printf("%d\t%d\n", errorCode, i);
        errorCount++;
        printError(i);
        //mcps[i].reset();
        resetI2C();
      }
    }
  }
}