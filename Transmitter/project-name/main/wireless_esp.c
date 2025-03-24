#include "main.h"
#include <esp_wifi.h>
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include <stdio.h>


void wirelessmessagetest(){
    print_msg("wireless test\n");
}

void read_mac_address(){
    uint8_t MacAddr[6];
    print_msg("Reading Mac Address ...\n");
    if(esp_wifi_get_mac(WIFI_IF_STA, MacAddr) == ESP_OK){
        print_msg("MAC Address is: ");
        for(int i =0; i <6; i++){
            print_num("%x ",MacAddr[i]);
        }
        print_msg("\n");
    }
    else{
        print_msg("Reading Mac Address Failed\n");
    }

}

void wifi_init() {
    esp_netif_init();
    esp_event_loop_create_default();
    nvs_flash_init();  // Initialize NVS
    
    //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    //esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
}
