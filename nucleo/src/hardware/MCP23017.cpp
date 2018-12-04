#include "hardware/MCP23017.h"

MCP23017::MCP23017(uint8_t adress, I2C &i2c_) : i2c(i2c_), adress(adress)
{
}

void MCP23017::init()
{
    char pointer[2] = {0x00};

    pointer[0] = MCP23017_IODIRA;
    i2c.write(MCP23017_ADDRESS, pointer, 2, 0);
    pointer[0] = MCP23017_IODIRB;
    i2c.write(MCP23017_ADDRESS, pointer, 2, 0);
}

uint8_t MCP23017::writeRegister(uint8_t reg, uint8_t data)
{
    char charArray[2] = {reg, data};
    return i2c.write((MCP23017_ADDRESS+ adress) << 1, charArray, 2);
}

uint8_t MCP23017::readRegister(uint8_t reg)
{
    char charArray[1] = {reg};
    char ret = 0;
    i2c.write((MCP23017_ADDRESS + adress) << 1, charArray, 1, 0);
    i2c.read((MCP23017_ADDRESS + adress) << 1, &ret, 1, 0);
    return ret;
}

void MCP23017::readGPIO(uint8_t reg)
{
    if (reg == MCP23017_GPIO_PORT_A)
    {
    }
    else if (reg == MCP23017_GPIO_PORT_B)
    {
    }
}

void MCP23017::writeGPIO(uint8_t reg, uint8_t data)
{
    if (reg == MCP23017_GPIO_PORT_A)
    {
        writeRegister(MCP23017_GPIOA, data);
    }
    else if (reg == MCP23017_GPIO_PORT_B)
    {
        writeRegister(MCP23017_GPIOB, data);
    }
}
