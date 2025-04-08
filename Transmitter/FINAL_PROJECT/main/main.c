#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "STEPPER_CONTROL"

// Define GPIOs for stepper motor
#define STEP_PIN GPIO_NUM_4  // PULSE pin
#define DIR_PIN  GPIO_NUM_19  // Direction pin

// Function to initialize GPIOs
void init_stepper() {
    gpio_reset_pin(STEP_PIN);
    gpio_reset_pin(DIR_PIN);

    gpio_set_direction(STEP_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIR_PIN, GPIO_MODE_OUTPUT);

    // Set initial state
    gpio_set_level(STEP_PIN, 0);
    gpio_set_level(DIR_PIN, 1);  // 1 = Forward, 0 = Reverse
}



void app_main() {
    ESP_LOGI(TAG, "Initializing Stepper...");
    init_stepper();

    while (1) {
       for ()
    }
}
