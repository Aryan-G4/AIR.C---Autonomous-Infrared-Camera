//Reciever CODE (GREEN ESP)
// MAC ADDR:  08:D1:F9:DD:54:3C


#include <stdio.h>
#include <string.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "ESP-NOW SLAVE";

// Callback function when data is received
void on_data_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    
    ESP_LOGI(TAG, "Received Data: %c", *(char*)data); // Print received integer
}

void app_main() {
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