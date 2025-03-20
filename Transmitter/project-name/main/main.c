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

#define I2C_MASTER_SCL_IO 22      // GPIO for SCL
#define I2C_MASTER_SDA_IO 21      // GPIO for SDA
#define I2C_MASTER_FREQ_HZ 100000 // 100kHz I2C speed
#define I2C_MASTER_PORT I2C_NUM_0 // Use I2C port 0
#define DEVICE_ADDR 0x50          // Replace with your device's I2C address
#define REGISTER_ADDR 0x10        // Replace with the register you want to read

/*Function declarations*/
void uart_init();
void i2c_init();
void print_msg(char* message);

void app_main() {
    uart_init(); // Initialize UART
    i2c_init();
    //char message[100];
    //int num = 3;

    //sprintf(message, "number is: %d\n", num);

    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    printf("testing printf");
    while (1) {
        print_msg("hi\n");
        //uart_write_bytes(UART_NUM, message, strlen(message)); // Send message over UART
        gpio_set_level(LED_PIN, 1);  // Turn LED ON
        vTaskDelay(pdMS_TO_TICKS(500));  // Wait 500ms

        gpio_set_level(LED_PIN, 0);  // Turn LED OFF
        vTaskDelay(pdMS_TO_TICKS(3000)); // Delay 1 second
    }
}

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

void i2c_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_PORT, &conf);
    i2c_driver_install(I2C_MASTER_PORT, conf.mode, 0, 0, 0);
}

void print_msg(char* message){
    uart_write_bytes(UART_NUM, message, strlen(message));
}