/**
 * @file Pomodoro-Timer.c
 * @brief Implementation of a Pomodoro Timer using Raspberry Pi Pico.
 *
 * This file contains the main logic for a Pomodoro Timer, including
 * initialization, timer callbacks, and GPIO interrupt handling.
 * 
 * @include "pico/stdlib.h"
 * @include "pico/time.h"
 * @include "hardware/timer.h"
 * @include "hardware_init.h"
 * @include "display_status.h"
 *
 * @function gpio_irq_handler(uint gpio, uint32_t events)
 * Interrupt handler for GPIO events.
 *
 * @function timer_callback(repeating_timer_t *rt)
 * Callback function for the repeating timer.
 *
 * @function inactive_timer_callback(repeating_timer_t *rt)
 * Callback function for the inactive timer.
 *
 * @function adjust_time(bool is_work_time)
 * Adjusts the timer based on whether it is work time or break time.
 *
 * @var default_work_minutes
 * Default duration for work periods in minutes.
 *
 * @var default_break_minutes
 * Default duration for break periods in minutes.
 *
 * @var work_minutes
 * Current duration for work periods in minutes.
 *
 * @var break_minutes
 * Current duration for break periods in minutes.
 *
 * @var minutes
 * Current minute count for the timer.
 *
 * @var seconds
 * Current second count for the timer.
 *
 * @var on_break
 * Flag indicating if the timer is currently in a break period.
 *
 * @var timer_running
 * Flag indicating if the timer is currently running.
 *
 * @var timer_on
 * Flag indicating if the timer is currently on.
 *
 * @var timer
 * Repeating timer structure for the main timer.
 *
 * @var inactive_timer
 * Repeating timer structure for the inactive timer.
 *
 * @var ssd
 * External variable for the SSD1306 display.
 *
 * @function main()
 * Main function that initializes the system and enters the main loop.
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "hardware_init.h"
#include "display_status.h"

// Prototypes
void gpio_irq_handler(uint gpio, uint32_t events);
bool timer_callback(repeating_timer_t *rt);
bool inactive_timer_callback(repeating_timer_t *rt);
void adjust_time(bool is_work_time);

// Variables
int default_work_minutes = 25;
int default_break_minutes = 5;
int work_minutes = 25;
int break_minutes = 5;
int minutes = 25;
int seconds = 0;
bool on_break = false;
bool timer_running = false;
bool timer_on = false;
repeating_timer_t timer;
repeating_timer_t inactive_timer;
extern ssd1306_t ssd;

int main()
{
    stdio_init_all();
    hardware_init();

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BUTTON_JS, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        sleep_ms(1000);
    }
}

/**
 * @brief GPIO interrupt handler for the Pomodoro Timer.
 *
 * This function handles GPIO interrupts for the Pomodoro Timer. It debounces
 * the button presses and performs actions based on which button was pressed.
 *
 * @param gpio The GPIO pin number that triggered the interrupt.
 * @param events The event type that triggered the interrupt.
 *
 * - BUTTON_A: Starts the Pomodoro timer if it is not already running. If the timer
 *   is on a break, it sets the LED to blue; otherwise, it sets the LED to green.
 * - BUTTON_B: Pauses the Pomodoro timer if it is running. If the timer is not running
 *   and not on, it adjusts the work time.
 * - BUTTON_JS: Stops the Pomodoro timer if it is on. Resets the timer to the default
 *   work time and turns off the LEDs. If the timer is not on, it adjusts the break time.
 *
 * The function also updates the display and manages the timer state.
 */
void gpio_irq_handler(uint gpio, uint32_t events) 
{
    static absolute_time_t last_interrupt_time = {0};
    absolute_time_t current_time = get_absolute_time();
    int64_t time_diff = absolute_time_diff_us(last_interrupt_time, current_time);

    if (time_diff < 300000) 
    { // 300ms debounce time
        return;
    }

    last_interrupt_time = current_time;

    if (gpio == BUTTON_A) {
        if (timer_running) {
            printf("Pomodoro already running\n");
            return;
        }

        add_repeating_timer_ms(-1000, timer_callback, NULL, &timer);
        if (on_break) {
            gpio_put(LED_RED, 0);
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_GREEN, 0);
        } else {
            gpio_put(LED_RED, 0);
            gpio_put(LED_BLUE, 0);
            gpio_put(LED_GREEN, 1);
        }

        timer_running = true;
        timer_on = true;
        printf("Pomodoro started\n");
        update_timer(minutes, seconds, on_break);
        return;
    } else if (gpio == BUTTON_B) {
        if (timer_running) {
            printf("Pomodoro paused\n");
            timer_running = false;
            gpio_put(LED_BLUE, 0);
            gpio_put(LED_GREEN, 1);
            gpio_put(LED_RED, 1);
            cancel_repeating_timer(&timer);
            ssd1306_draw_string(&ssd, "Paused", 60, 10);
            ssd1306_send_data(&ssd);
            return;
        } else if (!timer_on) {
            adjust_time(true); // Ajustar tempo de trabalho
        }
    } else if (gpio == BUTTON_JS) {
        if (timer_on) {
            printf("Pomodoro finished\n");
            minutes = default_work_minutes;
            seconds = 0;

            timer_running = false;
            timer_on = false;
            on_break = false;

            gpio_put(LED_RED, 0);
            gpio_put(LED_GREEN, 0);

            cancel_repeating_timer(&timer);
            initial_display();
            return;
        } else if (!timer_on) {
            adjust_time(false); // Ajustar tempo de pausa
        }
    }
}

/**
 * @brief Adjusts the timer settings for work or break periods.
 *
 * This function updates the default work or break minutes based on the input parameter.
 * It also updates the display to show the new settings and resets the timer.
 *
 * @param is_work_time A boolean value indicating whether to adjust the work time (true) or break time (false).
 *
 * The function performs the following steps:
 * - Clears the SSD1306 display.
 * - Draws a rectangle on the display.
 * - If `is_work_time` is true:
 *   - Increments the default work minutes by 1.
 *   - Resets the default work minutes to 1 if it exceeds 60.
 *   - Prints the new work time to the console.
 *   - Updates the display with the new work time.
 * - If `is_work_time` is false:
 *   - Increments the default break minutes by 1.
 *   - Resets the default break minutes to 1 if it exceeds 30.
 *   - Prints the new break time to the console.
 *   - Updates the display with the new break time.
 * - Updates the global variables for minutes, work_minutes, and break_minutes.
 * - Sends the updated data to the SSD1306 display.
 * - Cancels the inactive timer and sets a new repeating timer with a 4000 ms interval.
 */
void adjust_time(bool is_work_time) {
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, true, false);

    if (is_work_time) {
        default_work_minutes += 1;
        if (default_work_minutes > 60) default_work_minutes = 1;
        printf("Work time set to %d minutes\n", default_work_minutes);
        ssd1306_draw_string(&ssd, "Work time set ", 10, 10);
        ssd1306_draw_string(&ssd, "to", 10, 20);
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d minutes", default_work_minutes);
        ssd1306_draw_string(&ssd, buffer, 10, 30);
    } else {
        default_break_minutes += 1;
        if (default_break_minutes > 30) default_break_minutes = 1;
        printf("Break time set to %d minutes\n", default_break_minutes);
        ssd1306_draw_string(&ssd, "Break time set", 10, 10);
        ssd1306_draw_string(&ssd, "to", 10, 20);
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d minutes", default_break_minutes);
        ssd1306_draw_string(&ssd, buffer, 10, 30);
    }

    minutes = default_work_minutes;
    work_minutes = default_work_minutes;
    break_minutes = default_break_minutes;

    ssd1306_send_data(&ssd);

    cancel_repeating_timer(&inactive_timer);
    add_repeating_timer_ms(4000, inactive_timer_callback, NULL, &inactive_timer);
}

/**
 * @brief Callback function for the repeating timer.
 *
 * This function is called periodically by the repeating timer. It updates the 
 * timer's state, switching between work and break periods, and updates the 
 * LED indicators accordingly.
 *
 * @param rt Pointer to the repeating timer structure.
 * @return true to keep the timer running, false to stop it.
 */
bool timer_callback(repeating_timer_t *rt) 
{
    if (seconds == 0) {
        if (minutes == 0) {
            if (on_break) {
                minutes = work_minutes;
                seconds = 0;
                on_break = false;
                gpio_put(LED_GREEN, 1);
                gpio_put(LED_BLUE, 0);
                printf("Break finished\n");
            } else {
                minutes = break_minutes;
                seconds = 0;
                on_break = true;
                gpio_put(LED_GREEN, 0);
                gpio_put(LED_BLUE, 1);
                printf("Work finished\n");
            }
            return true;
        }
        minutes--;
        seconds = 59;
    } else {
        seconds--;
    }
    
    update_timer(minutes, seconds, on_break);
    return true;
}


/**
 * @brief Callback function for the inactive timer.
 *
 * This function is called when the timer is inactive. It initializes the display.
 *
 * @param rt Pointer to the repeating timer structure.
 * @return false to stop the timer.
 */
bool inactive_timer_callback(repeating_timer_t *rt) 
{
    initial_display();
    return false;
}