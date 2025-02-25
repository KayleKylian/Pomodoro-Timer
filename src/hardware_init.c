#include "hardware_init.h"
#include "display_status.h"

ssd1306_t ssd;

/**
 * @brief Initializes the hardware components required for the Pomodoro Timer.
 *
 * This function sets up the necessary hardware by initializing the button, display, and LED.
 * It also performs the initial display setup.
 *
 * The following components are initialized:
 * - Button: Prepares the button for user input.
 * - Display: Sets up the display for showing information.
 * - LED: Initializes the LED for visual feedback.
 * - Initial Display: Configures the initial state of the display.
 */
void hardware_init() {
    init_button();
    init_display();
    init_led();
    initial_display();
}

/**
 * @brief Initializes the buttons by setting up the GPIO pins.
 *
 * This function initializes the GPIO pins for BUTTON_A, BUTTON_B, and BUTTON_JS.
 * It sets the direction of these pins to input and enables the internal pull-up resistors.
 *
 * @note Ensure that the BUTTON_A, BUTTON_B, and BUTTON_JS macros are defined with the correct GPIO pin numbers.
 */
void init_button() {
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_init(BUTTON_JS);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_set_dir(BUTTON_JS, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);
    gpio_pull_up(BUTTON_JS);
}

/**
 * @brief Initializes the display hardware.
 *
 * This function sets up the necessary configurations and initializes
 * the display hardware for use in the Pomodoro Timer application.
 */
void init_display() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, 0x3C, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

/**
 * @brief Initializes the LED hardware.
 *
 * This function sets up the necessary configurations to initialize the LED
 * hardware on the device. It ensures that the LED is ready for use in the
 * application.
 */
void init_led() {
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    gpio_put(LED_RED, 0);
    gpio_put(LED_BLUE, 0);
    gpio_put(LED_GREEN, 0);
}