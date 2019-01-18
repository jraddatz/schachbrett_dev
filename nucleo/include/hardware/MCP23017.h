#include <mbed.h>

#ifndef MCP23017_H
#define MCP23017_H

#define MCP23017_GPIO_PORT_A 0x00
#define MCP23017_GPIO_PORT_B 0x01

#define MCP23017_ADDRESS 0x20

// registers
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14

#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

#define MCP23017_INT_ERR 255
/**
 * Hardware class for the MCP23017 GPIO Extender
 * PortA is for The LEDs<br>
 * PortB is for the hal sensor<br>
 * 
 * PortB pins 1-8  <br>
 * PortA pins 21-28<br>
 * VDD pin 9<br>
 * VSS pin 10<br>
 * SCL pin 12<br>
 * SDA pin 13<br>
 * A0,A1,A2 pins 15,16,17<br>
 */
class MCP23017 {
public:
  /**
  * Constructor which needs the ID of the MCP23017 and the I2C
  * @param id the ID of the MCP which is Based on the A0, A1, A2 configuration.
  * @param i2c_ the I2C interface the MCP23017 is connected with
  */
  MCP23017(uint8_t id, I2C & i2c_);
  /**
   * initialisation of the MCP23017. PortA gets configured as Output for the LEDs. PortB gets configured as Input for the Sensors and the Pullup are activated.
   */
  void init();
  /**
   * Write Data into a Register 
   * @param reg The Register to be rewritten
   * @param data the Data to write into the gien Register
   * @return uint8_t 
   */
  uint8_t writeRegister(uint8_t reg, uint8_t data);
  /**
   * Read current Value of a Register 
   * @param reg which Register
   * @return 
   */
  uint8_t readRegister(uint8_t reg);
  /**
   * @brief Reads directly from the designated Port
   * 
   * @param port which port should be read
   * @return uint8_t 
   */
  uint8_t readGPIO(uint8_t port);
  /**
   * @brief writes directly on the GPIO
   * 
   * @param port    which Port to be writen on
   * @param data    The Data which should be written
   * @return uint8_t 
   */
  uint8_t writeGPIO(uint8_t port, uint8_t data);
  /**
   * @brief Get the Changes which happened on the Port
   * 
   * @param port which Port should be checked
   * @return uint8_t 
   */
  uint8_t getChanges(uint8_t port);
private:
  I2C & i2c;
  uint8_t adress;
  uint8_t lastMeasurementA;
  uint8_t lastMeasurementB;
};

#endif