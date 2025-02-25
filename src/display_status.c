#include "display_status.h"
#include "../inc/ssd1306.h"
#include <stdio.h>

extern ssd1306_t ssd;

void initial_display()  {
    ssd1306_fill(&ssd, false);
    
    ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, true, false);
    ssd1306_draw_string(&ssd, "Pomodoro Timer", 10, 10);
    ssd1306_draw_string(&ssd, "A to start", 10, 30);
    ssd1306_draw_string(&ssd, "B to pause", 10, 40);
    ssd1306_send_data(&ssd);
}

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