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

bool inactive_timer_callback(repeating_timer_t *rt) 
{
    initial_display();
    return false;
}