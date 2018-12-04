#include <mbed.h>
#include <SerialWireOutput.h>
#include "hardware/MCP23017.h"



Serial pc(USBTX, USBRX); // tx, rx
I2C i2c(PB_9, PB_8);
DigitalOut pin (PB_1);
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




int main()
{
  i2c.frequency(400000);
  while (1)
  {


    wait_ms(100);
    // put your main code here, to run repeatedly:
  }
}