idf_component_register(SRCS "wireless_esp.c" "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver spi_flash esp_wifi esp_netif nvs_flash freertos esp_system)