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
#include "esp_log.h"
//MASTER CODE (Purple ESP)
//  purple ESP32 MAC Address: 08:D1:F9:2A:1B:54


// necessary files for writing code to interact with MLX90640 camera
#include "MLX90640_I2C_Driver.h" 
#include "MLX90640_API.h"

int curr_pos = 0;
int prev_pos = 0;

static const char *TAG = "ESP-NOW MASTER";
// ESP32-NOW Identity
uint8_t receiver_mac[] = {0x81, 0xD1, 0xF9, 0xDD, 0x54, 0x3C}; // REPLACE WITH YOUR SLAVE'S MAC
// Mac Address of the reciever
// float frame[NUM_ROWS*NUM_COLS]; // buffer for full frame of temperatures
// space for eeprom data to be stored
static uint16_t eeMLX90640[832]; //(NUM_ROWS+2)*NUM_COLS -- as described in driver pdf
// pointer to camera frame data already acquired
static uint16_t mlx90640Frame[834];
// pointer to MCU memory where already extracted params for device are stored (params decided by manufacturer)
paramsMLX90640 mlx90640;
static float mlx90640Image[NUM_ROWS*NUM_COLS]; //768
static float mlx90640Image_compare[NUM_ROWS*NUM_COLS]; //768

// uncomment *one* of the below
//#define PRINT_TEMPERATURES
#define PRINT_ASCIIART

void app_main() {
    char message[100];  // we'll use for all our printing 

    // initializing connections
    uart_init(); 
    MLX90640_I2CInit(); 
    wifi_init();
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_wifi_init(&(wifi_init_config_t)WIFI_INIT_CONFIG_DEFAULT());
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    esp_now_init();

    esp_now_register_send_cb(on_data_sent);
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, receiver_mac, 6);
    peer.channel = 0;
    peer.encrypt = false;

    if (esp_now_add_peer(&peer) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add peer\n");
        return;
    }
    sprintf(message, "Wireless Connection Enabled\n");
    esp_now_send(receiver_mac,(uint8_t*)message, sizeof(message));

    vTaskDelay(pdMS_TO_TICKS(3000));

    // set up alarm LED pin
    gpio_reset_pin(RED_LED_PIN);
    gpio_set_direction(RED_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RED_LED_PIN,0);

    // set up indication LED pin
    gpio_reset_pin(GREEN_LED_PIN);
    gpio_set_direction(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(GREEN_LED_PIN,0);

    // set up booting up LED pin
    gpio_reset_pin(YELLOW_LED_PIN);
    gpio_set_direction(YELLOW_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(YELLOW_LED_PIN,1);
    vTaskDelay(pdMS_TO_TICKS(5000));

    // set up motor pins
    gpio_reset_pin(STEP_PIN);
    gpio_set_direction(STEP_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(STEP_PIN,0);
    gpio_reset_pin(DIR_PIN);
    gpio_set_direction(DIR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DIR_PIN,0);

    // set up one-time settings
    MLX90640_DumpEE (DEVICE_ADDR, eeMLX90640);  // need to dump eeprom to get access the paramters
    MLX90640_SetResolution(DEVICE_ADDR, 0x03);  // 16bit resolution
    int curResolution;
    curResolution = MLX90640_GetCurResolution(DEVICE_ADDR);
    sprintf(message, "Current resultuion=%d bits\n", 16+curResolution);
    print_msg(message);
    MLX90640_SetRefreshRate (DEVICE_ADDR,0x02);         // previously tried 0x05            -- 1 frame per second
    int curRR;
    curRR = MLX90640_GetRefreshRate (DEVICE_ADDR);
    sprintf(message, "Current refresh rate=%d fps\n", curRR);  
    print_msg(message);
    
    MLX90640_SetChessMode (DEVICE_ADDR);    // chess mode is nicer -- grid rather than stacked lines
    int mode;
    mode = MLX90640_GetCurMode(0x33);
    sprintf(message,"current mode(%d)=%s\n",mode,(mode?"chess":"interleaved"));
    print_msg(message);

    MLX90640_ExtractParameters(eeMLX90640,&mlx90640);   // extracting them from the previous eeprom dump
    sprintf(message, "Extracting parameters done!\nVdd=%d\n", mlx90640.vdd25);
    print_msg(message);
    MLX90640_I2CFreqSet(400);

    MLX90640_GetFrameData(DEVICE_ADDR, mlx90640Frame);
    int subPage;
    subPage = MLX90640_GetSubPageNumber(mlx90640Frame);     // this is for testing moreso
    sprintf(message, "current subpage # is %d\n",subPage);
    print_msg(message);
    sprintf(message,"Vdd=%f\n",MLX90640_GetVdd(mlx90640Frame,&mlx90640));        // in data sheet example = -13115
    print_msg(message);

    MLX90640_GetImage(mlx90640Frame, &mlx90640, mlx90640Image);
    sprintf(message, "Device Initialized\n");
    esp_now_send(receiver_mac,(uint8_t*)message, sizeof(message));
    while (1) {
        // printf("In the main loop\n");
        //print_msg("hi\n");
        //uart_write_bytes(UART_NUM, message, strlen(message)); // Send message over UART
        // wirelessmessagetest();
        // read_mac_address();
        gpio_set_level(GREEN_LED_PIN,1);
        gpio_set_level(YELLOW_LED_PIN,0);

        MLX90640_GetFrameData(DEVICE_ADDR, mlx90640Frame);
        float ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
        sprintf(message, "Ambinet temperature=%f\n", ta);     // in testing = ~29 C
        print_msg(message);

        // gets the ACTUAL (calculated) temperature of object in C
        // emissivity (how reflective obj is) = 0.95
        // reflected temperature (tr) -- in driver pdf says that ta-8 is pretty standard
        MLX90640_CalculateTo(mlx90640Frame, &mlx90640, 0.95, ta-8, mlx90640Image);  
        float t_max=-1000; 
        float t_min=1000;
        for (uint8_t h=0; h<24; h++) {
            for (uint8_t w=0; w<32; w++) {
                float t = mlx90640Image[h*32 + w];
                // storing min/max temps -- for sanity check but also could use to set alarm trigger
                if(t>t_max) t_max=t;
                if(t<t_min) t_min=t;

                #ifdef PRINT_TEMPERATURES
                    Serial.print(t, 1);
                    Serial.print(", ");
                #endif
                #ifdef PRINT_ASCIIART
                /*   
                char c = '&';                           
                    if (t < 20) c = ' ';
                    else if (t < 23) c = '.';
                    else if (t < 25) c = '-';
                    else if (t < 27) c = '*';
                    else if (t < 29) c = '+';
                    else if (t < 31) c = 'x';
                    else if (t < 33) c = '%';
                    else if (t < 35) c = '#';
                    else if (t < 37) c = 'X';
                    // sprintf(message, "%c", c);     // for printing ascii map
                    sprintf(message, "%.3f,",t);      // for printing values -- can use in excel
                    print_msg(message);
                */
                #endif
            }
            sprintf(message, "\n");
            print_msg(message);
        }
        sprintf(message, "t_max=%f, t_min=%f\n", t_max, t_min);
        print_msg(message);
        
        while (t_max >= 130) {
            // based on observation, flame from lighter was about 147 degrees C
            // for safety, we will set the threshold to 130 degrees C
            // we know this will not conflict with body temp or LA summer temps (highest LA summer temp is 54.4)
            
            gpio_set_level(GREEN_LED_PIN,0);

            // stop moving motor
            // just wont take step here

            // send wireless message
            sprintf(message, "FIRE\tFIRE\tFIRE\n");   ///////////////////////CHANGE THIS TO WIRELESS TRANSMIT
            print_msg(message);
            sprintf(message,"FIRE DETECTED\t\n");
            esp_now_send(receiver_mac,(uint8_t*)message, sizeof(message));
            // blink LED lights x6
            toggleLED();

            // check if fire is still there -- ie updating t_max by taking another picture of the same location
            MLX90640_GetImage(mlx90640Frame, &mlx90640, mlx90640Image_compare);
            MLX90640_GetFrameData(DEVICE_ADDR, mlx90640Frame);
            float ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
            MLX90640_CalculateTo(mlx90640Frame, &mlx90640, 0.95, ta-8, mlx90640Image_compare);  
            float t_max_new=-1000; 
            float t_min_new=1000;
            for (uint8_t h=0; h<24; h++) {
                for (uint8_t w=0; w<32; w++) {
                    float t = mlx90640Image_compare[h*32 + w];
                    if(t>t_max_new) t_max_new=t;
                    if(t<t_min_new) t_min_new=t;
                }
            }
            t_max = t_max_new;  // if t_max_new is still >=130, the loop will just continue
        }
        sprintf(message,"no fire: all is good\t\n\n\n\n\n\n");
        esp_now_send(receiver_mac,(uint8_t*)message, sizeof(message));
        step_motor();
        vTaskDelay(pdMS_TO_TICKS(1000));
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

void print_msg(char* message){
    uart_write_bytes(UART_NUM, message, strlen(message));
}

void print_num(char*message, uint8_t num){
    char outbuff[30];
    sprintf(outbuff, message,num);
    uart_write_bytes(UART_NUM, outbuff, strlen(outbuff));
}

void toggleLED(void){
    // if (gpio_get_level(LED_PIN) == 1){
    //     gpio_set_level(LED_PIN,0);
    // }
    // else{
    //     gpio_set_level(LED_PIN,1);
    // }
    gpio_set_level(RED_LED_PIN,1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(RED_LED_PIN,0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(RED_LED_PIN,1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(RED_LED_PIN,0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(RED_LED_PIN,1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(RED_LED_PIN,0);
    vTaskDelay(pdMS_TO_TICKS(200));
}

// when data is sent
void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Send Status: %s", status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}


void step_motor() {
    char message[100];
    if (curr_pos == 0 && prev_pos == 0) {
        sprintf(message, "initial step: ccw\n");
        print_msg(message);
        step_ccw();
    } else if(curr_pos < prev_pos){
        sprintf(message, "changing direction: stepping ccw\n");
        print_msg(message);
        step_ccw();
    }
    else if (curr_pos > prev_pos){
        sprintf(message, "changing direction: stepping cw\n");
        print_msg(message);
        step_cw();
    }
}

void step_ccw(){
    char message[100];
    if(curr_pos ==-3){
        prev_pos = -4;
        gpio_set_level(DIR_PIN,1);
        step_cw();
        return;
    }
    sprintf(message, "curr pos (ccw): %d", curr_pos);
    print_msg(message);
    gpio_set_level(STEP_PIN,0);
    gpio_set_level(DIR_PIN,0);
    for(int i = 0; i<70;i++){
        gpio_set_level(STEP_PIN,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(STEP_PIN,0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    prev_pos = curr_pos;
    curr_pos--;
    return;
}

void step_cw(){
    char message[100];
    if(curr_pos ==3){
        prev_pos = 4;
        step_ccw();
        return;
    }
    sprintf(message, "curr pos (cw): %d", curr_pos);
    print_msg(message);
    gpio_set_level(STEP_PIN,0);
    gpio_set_level(DIR_PIN,1);
    for(int i = 0; i<70;i++){
        gpio_set_level(DIR_PIN,1);
        gpio_set_level(STEP_PIN,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(STEP_PIN,0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    prev_pos = curr_pos;
    curr_pos++;
    return;
}
