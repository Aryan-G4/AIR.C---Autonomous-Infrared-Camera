#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_mac.h"

void app_main() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_IF_WIFI_STA);
    printf("ESP32 MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}