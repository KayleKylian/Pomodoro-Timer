/**
 * @file hardware_init.h
 * @brief Header file for hardware initialization functions and definitions.
 *
 * This file contains the definitions and function prototypes for initializing
 * the hardware components used in the Pomodoro Timer project. It includes
 * initialization for buttons, LEDs, and the I2C display.
 *
 * @author Italo
 * @date 2023
 */

#ifndef HARDWARE_INIT_H
#define HARDWARE_INIT_H

#include <pico/stdlib.h>
#include "hardware/i2c.h"
#include "../inc/ssd1306.h"
#include "../inc/font.h"

#define BUTTON_A 5    ///< GPIO pin for Button A
#define BUTTON_B 6    ///< GPIO pin for Button B
#define BUTTON_JS 22  ///< GPIO pin for Joystick Button

#define LED_RED 13    ///< GPIO pin for Red LED
#define LED_BLUE 12   ///< GPIO pin for Blue LED
#define LED_GREEN 11  ///< GPIO pin for Green LED

// I2C defines
#define I2C_PORT i2c1 ///< I2C port used for communication
#define I2C_SDA 14    ///< GPIO pin for I2C SDA
#define I2C_SCL 15    ///< GPIO pin for I2C SCL

/**
 * @brief Initializes the hardware components.
 *
 * This function initializes the hardware components including buttons, LEDs,
 * and the I2C display.
 */
void hardware_init(void);

/**
 * @brief Initializes the buttons.
 *
 * This function sets up the GPIO pins for the buttons and configures them
 * as inputs.
 */
void init_button(void);

/**
 * @brief Initializes the display.
 *
 * This function sets up the I2C communication and initializes the SSD1306
 * display.
 */
void init_display(void);

/**
 * @brief Initializes the LEDs.
 *
 * This function sets up the GPIO pins for the LEDs and configures them
 * as outputs.
 */
void init_led(void);

#endif // HARDWARE_INIT_H