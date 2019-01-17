

#ifndef Board_H
#define Board_H

#include <mbed.h>
#include "consts.h"
#include "hardware/MCP23017.h"
#include "coords.h"

class Board {
public:
    /**
     * @brief Construct a new Board object
     * 
     */
    Board();
    /**
     * @brief Initialisation of the Board 
     * configurates the I2c, Notreset, MCPs
     */
    void init();
    /**
     * toggle an LED at the given coordinates
     * @param x the x coordinate of the LED
     * @param y the y coordinate of the LED
     * @oaram on 1 for on, 0 for off (default is 1)
     *    
     */
    void ledToggle(uint8_t x, uint8_t y, uint8_t on = 1);
    /**
     * returns the status of the field at given coordinates
     * @param x the x coordinate of the field
     * @param y the y coordinate of the field
     * @return 1 for occupied, 0 for empty
     */ 
    uint8_t checkField(uint8_t x, uint8_t y);
    /**
      * Toggles all LEDs off
      */
    void ledsOff();
    /**
     * @brief 
     * 
     */
    void updateAll();
    /**
     * Helper Function to reset all GPIO Extender
     */
    void resetI2C();
    /**
     * @brief Get the Change object
     * 
     * @return coords 
     */
    coords getChange();
private:
    MCP23017 mcps[8];
    DigitalOut i2cReset;
    I2C i2c;
};

#endif