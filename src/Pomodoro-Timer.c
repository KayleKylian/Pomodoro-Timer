#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "hardware_init.h"
#include "display_status.h"

// Prototypes
void gpio_irq_handler(uint gpio, uint32_t events);
bool timer_callback(repeating_timer_t *rt);

// Variables
int work_minutes = 25;
int break_minutes = 5;
int minutes = 1;
int seconds = 0;
bool on_break = false;
bool timer_running = false;
repeating_timer_t timer;
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
        printf("Pomodoro started\n");
        update_timer(minutes, seconds, on_break); // Update display immediately
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
        }
    } else if (gpio == BUTTON_JS) {
        printf("Pomodoro finished\n");
        minutes = work_minutes;
        seconds = 0;
        timer_running = false;
        on_break = false;
        gpio_put(LED_RED, 0);
        gpio_put(LED_GREEN, 0);
        cancel_repeating_timer(&timer);
        initial_display();
        return;
    }
}

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