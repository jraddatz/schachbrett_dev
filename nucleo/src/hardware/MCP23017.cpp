#include "hardware/MCP23017.h"
#include "mbed.h"

MCP23017::MCP23017(uint8_t adress, I2C &i2c_) : i2c(i2c_), adress(adress)
{
}

void MCP23017::init()
{
    writeRegister (MCP23017_IODIRA, 0x00);
    writeRegister (MCP23017_IODIRB, 0xFF);
    writeRegister (MCP23017_GPPUB, 0xFF);
}

uint8_t MCP23017::writeRegister(uint8_t reg, uint8_t data)
{
    char charArray[2] = {reg, data};
    return i2c.write((MCP23017_ADDRESS + adress) << 1, charArray, 2);
}

uint8_t MCP23017::readRegister(uint8_t reg)
{
    char charArray[1] = {reg};
    char ret = 0;
    i2c.write((MCP23017_ADDRESS + adress) << 1, charArray, 1, false);
    i2c.read((MCP23017_ADDRESS + adress) << 1, &ret, 1, false);
    return ret;
}

uint8_t MCP23017::readGPIO(uint8_t reg)
{
    if (reg == MCP23017_GPIO_PORT_A)
    {
        return readRegister(MCP23017_GPIOA);
    }
    else if (reg == MCP23017_GPIO_PORT_B)
    {
        return readRegister(MCP23017_GPIOB);
    }
    return -1;
}

uint8_t MCP23017::writeGPIO(uint8_t reg, uint8_t data)
{
    if (reg == MCP23017_GPIO_PORT_A)
    {
       return writeRegister(MCP23017_GPIOA, data);
    }
    else if (reg == MCP23017_GPIO_PORT_B)
    {
       return writeRegister(MCP23017_GPIOB, data);
    }
    return -1;    
}

uint8_t MCP23017::getChanges(uint8_t port){
    uint8_t diff = 0;
    if(port == MCP23017_GPIO_PORT_A){
        diff = lastMeasurementA;
        lastMeasurementA = readGPIO(port);
        diff ^= lastMeasurementA;
    } else if (port == MCP23017_GPIO_PORT_B) {
        diff = lastMeasurementB;
        lastMeasurementB = readGPIO(port);
        diff ^= lastMeasurementB;
    }
    return diff;
}
