#include "display_status.h"
#include "../inc/ssd1306.h"
#include <stdio.h>

extern ssd1306_t ssd;

/**
 * @brief Initializes the display with the initial screen for the Pomodoro Timer.
 * 
 * This function clears the display, draws a border around the screen, and displays
 * the initial instructions for the Pomodoro Timer. The instructions include:
 * - "Pomodoro Timer" at coordinates (10, 10)
 * - "A to start" at coordinates (10, 30)
 * - "B to pause" at coordinates (10, 40)
 * 
 * The function then sends the data to the display to update it.
 */
void initial_display()  {
    ssd1306_fill(&ssd, false);
    
    ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, true, false);
    ssd1306_draw_string(&ssd, "Pomodoro Timer", 10, 10);
    ssd1306_draw_string(&ssd, "A to start", 10, 30);
    ssd1306_draw_string(&ssd, "B to pause", 10, 40);
    ssd1306_send_data(&ssd);
}

/**
 * @brief Updates the timer display on the SSD1306 OLED screen.
 *
 * This function updates the timer display with the given minutes and seconds.
 * It also indicates whether the timer is in a break period or a work period.
 *
 * @param minutes The number of minutes to display.
 * @param seconds The number of seconds to display.
 * @param on_break A boolean indicating if the timer is in a break period (true) or a work period (false).
 */
void update_timer(int minutes, int seconds, bool on_break) {
    char timer[16];
    snprintf(timer, sizeof(timer), "%02d:%02d", minutes, seconds);

    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, true, false);
    if (on_break) {
        ssd1306_draw_string(&ssd, "Break", 10, 10);
    } else {
        ssd1306_draw_string(&ssd, "Work", 10, 10);
    }
    ssd1306_draw_string(&ssd, timer, 10, 30);
    ssd1306_send_data(&ssd);
}