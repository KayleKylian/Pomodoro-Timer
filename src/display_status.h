/**
 * @file display_status.h
 * @brief Header file for display status functions.
 *
 * This file contains the declarations for functions that handle the display
 * status of the Pomodoro Timer application.
 */

#ifndef DISPLAY_STATUS_H
#define DISPLAY_STATUS_H

#include <stdbool.h>

/**
 * @brief Initializes the display.
 *
 * This function sets up the initial state of the display.
 */
void initial_display(void);

/**
 * @brief Updates the timer display.
 *
 * This function updates the display with the current timer values.
 *
 * @param minutes The number of minutes remaining.
 * @param seconds The number of seconds remaining.
 * @param on_break A boolean indicating if the timer is on a break.
 */
void update_timer(int minutes, int seconds, bool on_break);

#endif // DISPLAY_STATUS_H