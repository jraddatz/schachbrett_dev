#ifndef coords_h
#define coords_h
#include <mbed.h>

/**
 * struct for the move of a single figure
 */
typedef struct {
  uint8_t x;
  uint8_t y;
  bool up;
} coords;


#endif