#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// UART Configuration
#define UART_NUM UART_NUM_0
#define TXD_PIN 17
#define RXD_PIN 16
#define BUF_SIZE 1024

// LED GPIO
#define LED_PIN 2

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
uint16_t i2c_read(uint16_t reg);
void i2c_write(uint16_t data, uint16_t reg);
void print_arr(int *arr, int rows, int cols);

#endif // MAIN_H