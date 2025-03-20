#include <stdio.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"



#define UART_NUM UART_NUM_0  // Use UART2
#define TXD_PIN 17           // GPIO17 as TX
#define RXD_PIN 16           // GPIO16 as RX
#define BUF_SIZE 1024        // Buffer size
#define LED_PIN 2

void uart_init() {
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 115200,    // Set baud rate
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    // Install UART driver and configure pins
    uart_driver_install(UART_NUM, BUF_SIZE, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}



void app_main() {
    uart_init(); // Initialize UART
    char message[100];
    int num = 4;

    sprintf(message, "number is: %d\n", num);

    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    
    while (1) {
        uart_write_bytes(UART_NUM, message, strlen(message)); // Send message over UART
        gpio_set_level(LED_PIN, 1);  // Turn LED ON
        vTaskDelay(pdMS_TO_TICKS(500));  // Wait 500ms

        gpio_set_level(LED_PIN, 0);  // Turn LED OFF
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second
    }
}