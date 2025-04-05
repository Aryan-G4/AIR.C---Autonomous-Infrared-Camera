#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "esp_now.h"

// UART Configuration
#define UART_NUM UART_NUM_0
#define TXD_PIN 17
#define RXD_PIN 16
#define BUF_SIZE 1024

// LED GPIO
#define RED_LED_PIN GPIO_NUM_17
#define GREEN_LED_PIN GPIO_NUM_5
#define YELLOW_LED_PIN GPIO_NUM_16

// PINS FOR MOTOR
#define STEP_PIN GPIO_NUM_4
#define DIR_PIN GPIO_NUM_23

// I2C Configuration
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_PORT I2C_NUM_0
#define DEVICE_ADDR 0x33

// Sensor Configuration
#define NUM_ROWS 24
#define NUM_COLS 32

// Important Register Definitions
#define STATUS_REG 0x8000

// Function Declarations
void uart_init();
void i2c_init();
void print_msg(char* message);
void print_num(char * message, uint8_t num);
uint16_t i2c_read(uint16_t reg);
void i2c_write(uint16_t data, uint16_t reg);

void print_arr(int *arr, int rows, int cols);
void toggleLED();
void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status);
void step_motor();
void step_ccw();
void step_cw();


#endif // MAIN_H