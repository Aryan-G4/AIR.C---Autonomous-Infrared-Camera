idf_component_register(SRCS "wireless_esp.c" "main.c" "MLX90640_API.c" "MLX90640_I2C_Driver.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver spi_flash esp_wifi esp_netif nvs_flash freertos esp_system)