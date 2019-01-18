#include "Board.h"

Board::Board() : mcps({
                              MCP23017(0, i2c), MCP23017(1, i2c),
                              MCP23017(2, i2c), MCP23017(3, i2c),
                              MCP23017(4, i2c), MCP23017(5, i2c),
                              MCP23017(6, i2c), MCP23017(7, i2c)
                      }), i2cReset(constants::PIN_I2C_NOTRESET),
                 i2c(constants::PIN_I2C_DATA, constants::PIN_I2C_CLOCK) {

}

void Board::init() {
    i2c.frequency(100000);
    resetI2C();
}

void Board::ledToggle(uint8_t x, uint8_t y, uint8_t on) {
    if (x >= 8 || y >= 8) {
        return;
    }
    uint8_t a = mcps[x].readGPIO(MCP23017_GPIO_PORT_A);
    a = on ? a | 1 << y : a & ~(1 << y);
    mcps[x].writeGPIO(MCP23017_GPIO_PORT_A, a);
}

uint8_t Board::checkField(uint8_t x, uint8_t y) {
    if (x >= 8 || y >= 8) {
        return -1; // error
    }
    uint8_t a = mcps[x].readGPIO(MCP23017_GPIO_PORT_B);
    return (a >> y & 1) ^ 1;
}

void Board::ledsOff() {
    uint8_t errorCode;
    for (uint8_t x = 0; x < 8; x++) {
        errorCode = mcps[x].writeGPIO(MCP23017_GPIO_PORT_A, 0x00);
        if (errorCode) {
            // reset();
        }
    }
}

void Board::resetI2C() {
    i2cReset = 0;
    wait_ms(1);
    i2cReset = 1;
    for (int i = 0; i < 8; i++) {
        mcps[i].init();
    }

}

void Board::updateAll() {
    for (uint8_t y = 0; y < 8; y++) {
        mcps[y].getChanges(MCP23017_GPIO_PORT_B);
    }
}

coords Board::getChange() {
    coords ret;
    ret.x = 255;
    uint8_t change;
    for (uint8_t x = 0; x < 8; x++) {
        change = mcps[x].getChanges(MCP23017_GPIO_PORT_B);
        if (change) {
            uint8_t a = mcps[x].readGPIO(MCP23017_GPIO_PORT_B);
            uint8_t y = 0;
            while (change) {
                if (change & 1) {
                    coords ret;
                    ret.x = x;
                    ret.y = y;
                    ret.up = (a & 1 << y) != 0;
                    printf("x = %d; y = %d; down = %u\n", x, y, (a >> y & 1));
                    return ret;
                }
                y++;
                change >>= 1;
            }
        }
    }
    return ret;
}