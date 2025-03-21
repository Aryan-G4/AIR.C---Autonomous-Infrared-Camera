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
#define DEVICE_ADDR 0x33           // Replace with your device's I2C address

/*Function declarations*/
void uart_init();
void i2c_init();
void print_msg(char* message);
uint16_t i2c_read(uint16_t reg);
void i2c_write(uint16_t data, uint16_t reg);


void app_main() {
    uart_init(); // Initialize UART
    i2c_init();
    char message[100];
    //int num = 3;

    //sprintf(message, "number is: %d\n", num);

    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    printf("testing printf");
    
    while (1) {
        print_msg("hi\n");
        //uart_write_bytes(UART_NUM, message, strlen(message)); // Send message over UART

        uint16_t pixelval = 1;
        pixelval = i2c_read(0x058A);
        
        sprintf(message, "i2c read completed, pixel value is: %x\n",pixelval);
        print_msg(message);
        //vTaskDelay(10000);
        //i2c_write(0x0001,0x8000);
        gpio_set_level(LED_PIN, 1);  // Turn LED ON
        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay 1 second
        gpio_set_level(LED_PIN, 0);  // Turn LED OFF
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

uint16_t i2c_read(uint16_t reg){
    print_msg("starting i2c read\n");
    uint8_t buffer[2] = {(reg&0xFF00)>>8, reg & 0x00FF};
    uint8_t result[2];
    print_msg("entering i2c master write\n");
    if(i2c_master_write_to_device(I2C_MASTER_PORT,DEVICE_ADDR,buffer,2,5000) != ESP_OK){
        print_msg("i2c master write failed\n");
    }
    print_msg("completed i2c master write\n");
    
    if(i2c_master_read_from_device(I2C_MASTER_PORT,DEVICE_ADDR,result,2,5000) != ESP_OK){
        print_msg("i2c master read failed\n");
    }
    print_msg("finished I2c read from reg\n");
    uint16_t end = result[0] | result[1]<<8;
    return end;

}

void i2c_write(uint16_t data,uint16_t reg) {
    print_msg("starting i2c write\n");
    uint8_t buffer[4] = {(reg&0xFF00)>>8, reg & 0x00FF, (data&0xFF00)>>8, data & 0x00FF};   // buffer contains address and data to be written

    print_msg("entering 12c master write\n");
    if(i2c_master_write_to_device(I2C_MASTER_PORT, DEVICE_ADDR, buffer, 4, 5000)!=ESP_OK){
        print_msg("i2c master read failed\n");
    };
    print_msg("completed i2c master write\n");
}

void print_arr(int *arr, int rows, int cols) {
    // print out our array of temperature values
    print_msg("\t\tSTART OF CURRENT TEMPERATURE MAP\n");
    char message[100];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            sprintf(message, "%d ", arr[i*cols + j]);
            print_msg(message);
        }
        print_msg("\n");
    }
    print_msg("\t\tEND OF CURRENT TEMPERATURE MAP\n");
}