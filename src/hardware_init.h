#ifndef HARDWARE_INIT_H
#define HARDWARE_INIT_H

#include <pico/stdlib.h>
#include "hardware/i2c.h"
#include "../inc/ssd1306.h"
#include "../inc/font.h"

#define BUTTON_A 5
#define BUTTON_B 6
#define BUTTON_JS 22

#define LED_RED 13
#define LED_BLUE 12
#define LED_GREEN 11

// I2C defines
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

// Function to initialize the hardware
void hardware_init(void);

// Function to initialize the button
void init_button(void);

// Function to initialize the display
void init_display(void);

// Function to initialize the LED
void init_led(void);

#endif // HARDWARE_INIT_H