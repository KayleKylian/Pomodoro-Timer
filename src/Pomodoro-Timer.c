#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware_init.h"

// Prototypes
void gpio_irq_handler(uint gpio, uint32_t events);

int main()
{
    stdio_init_all();
    hardware_init();

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A) {
        printf("Button A pressed\n");
    } else if (gpio == BUTTON_B) {
        printf("Button B pressed\n");
    }
}