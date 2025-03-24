#include <stdio.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "wireless_esp.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "main.h"

void app_main() {
    uart_init(); // Initialize UART
    i2c_init();
    wifi_init();


    char message[100];
    //int num = 3;

    //sprintf(message, "number is: %d\n", num);

    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    
    while (1) {
        //print_msg("hi\n");
        //uart_write_bytes(UART_NUM, message, strlen(message)); // Send message over UART
        wirelessmessagetest();
        read_mac_address();
        vTaskDelay(pdMS_TO_TICKS(3000));
        
        uint16_t pixelval = 1;
        pixelval = i2c_read(0x0486);
        
        sprintf(message, "i2c read completed, pixel value is: 0x%x\n",pixelval);
        print_msg(message);

        print_msg("STARTING TO LOOK THROUGH ARRAY\n");
        for(uint16_t i =0; i < 0x07FF; i+=4){
            uint16_t regdata = i2c_read(i);
            sprintf(message, "register %x value :%x\n",i,regdata);
            print_msg(message);
        }
        print_msg("DONE reading registers\n");
        //while(1);
        
        uint16_t cur_status = i2c_read(STATUS_REG);
        uint16_t new_status = cur_status & 0xFFF7;
        i2c_write(new_status,STATUS_REG);

        
         cur_status = i2c_read(STATUS_REG);
         new_status = cur_status | 0x0020;
        i2c_write(new_status,STATUS_REG); 

        while((i2c_read(STATUS_REG) & 0x0008) == 0){
            print_msg("waiting for status bit\n");
            vTaskDelay(100);
        }

        toggleLED();
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second
        toggleLED();
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

void print_num(char*message, uint8_t num){
    char outbuff[30];
    sprintf(outbuff, message,num);
    uart_write_bytes(UART_NUM, outbuff, strlen(outbuff));
}

uint16_t i2c_read(uint16_t reg){
    //print_msg("starting i2c read\n");
    uint8_t buffer[2] = {(reg&0xFF00)>>8, reg & 0x00FF};
    uint8_t result[2];
    //print_msg("entering i2c master write\n");
    if(i2c_master_write_to_device(I2C_MASTER_PORT,DEVICE_ADDR,buffer,2,5000) != ESP_OK){
        print_msg("i2c master write failed\n");
    }
    //print_msg("completed i2c master write\n");
    vTaskDelay(pdMS_TO_TICKS(10));
    if(i2c_master_read_from_device(I2C_MASTER_PORT,DEVICE_ADDR,result,2,5000) != ESP_OK){
        print_msg("i2c master read failed\n");
    }
    //print_msg("finished I2c read from reg\n");
    return ((uint16_t)result[0] << 8) | result[1];

}

void i2c_write(uint16_t data,uint16_t reg) {
    //print_msg("starting i2c write\n");
    uint8_t buffer[4] = {(reg>>8)&0xFF, reg & 0x00FF, (data>>8)&0xFF, data & 0x00FF};   // buffer contains address and data to be written

    //print_msg("entering 12c master write\n");
    if(i2c_master_write_to_device(I2C_MASTER_PORT, DEVICE_ADDR, buffer, 4, 5000)!=ESP_OK){
        print_msg("i2c master read failed\n");
    };
    //print_msg("completed i2c master write\n");
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

void toggleLED(void){
    if (gpio_get_level(LED_PIN)){
        gpio_set_level(LED_PIN,0);
    }
    else{
        gpio_set_level(LED_PIN,1);
    }
}