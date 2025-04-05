
#include <stdio.h>
#include <string.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"

//Reciever CODE (GREEN ESP)
// MAC ADDR:  08:D1:F9:DD:54:3C

static const char *TAG = "ESP-NOW SLAVE";

void print_msg(char* message){
    uart_write_bytes(UART_NUM_0, message, strlen(message));
}

// Callback function when data is received
void on_data_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    char * message = (char*)data;
    print_msg(message);
    //ESP_LOGI(TAG, "Received Data: %c", *(char*)data); // Print received integer
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
    uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void app_main() {
    uart_init();
    ESP_ERROR_CHECK(nvs_flash_init()); // Initialize flash storage (needed for ESP-NOW)
    ESP_ERROR_CHECK(esp_netif_init()); // Initialize networking stack
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Create event loop

    ESP_ERROR_CHECK(esp_wifi_init(&(wifi_init_config_t)WIFI_INIT_CONFIG_DEFAULT()));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); // Set Wi-Fi mode to Station
    ESP_ERROR_CHECK(esp_wifi_start()); // Start Wi-Fi (even though it won't connect)

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW Init Failed");
        return;
    }

    // Register callback for received data
    esp_now_register_recv_cb(on_data_recv);

    ESP_LOGI(TAG, "ESP-NOW Ready. Waiting for data...");
    
}