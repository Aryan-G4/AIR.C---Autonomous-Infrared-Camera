#include <stdio.h>
#include <string.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

//MASTER CODE (Purple ESP)
//  purple ESP32 MAC Address: 08:D1:F9:2A:1B:54

static const char *TAG = "ESP-NOW MASTER";

uint8_t receiver_mac[] = {0x81, 0xD1, 0xF9, 0xDD, 0x54, 0x3C}; // REPLACE WITH YOUR SLAVE'S MAC

// when data is sent
void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Send Status: %s", status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init()); // Initialize flash storage
    ESP_ERROR_CHECK(esp_netif_init()); // Initialize networking stack
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Create event loop

    ESP_ERROR_CHECK(esp_wifi_init(&(wifi_init_config_t)WIFI_INIT_CONFIG_DEFAULT()));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); // Set Wi-Fi mode to Station
    ESP_ERROR_CHECK(esp_wifi_start()); // Start Wi-Fi

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW Init Failed");
        return;
    }

    // Register send callback
    esp_now_register_send_cb(on_data_sent);

   
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, receiver_mac, 6);
    peer.channel = 0;
    peer.encrypt = false;

    if (esp_now_add_peer(&peer) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add peer");
        return;
    }

    // Send data every 2 seconds
    char message[100] = "ABC\n";
    while (1) {
        
        esp_err_t result = esp_now_send(receiver_mac, (uint8_t *) &message, sizeof(message));

        if (result == ESP_OK) {
            ESP_LOGI(TAG, "Sent Data:");
        } else {
            ESP_LOGE(TAG, "Failed to send");
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        
    }
}
