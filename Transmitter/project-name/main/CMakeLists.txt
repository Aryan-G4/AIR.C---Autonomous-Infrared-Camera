idf_component_register(SRCS "wireless_esp.c" "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver spi_flash)