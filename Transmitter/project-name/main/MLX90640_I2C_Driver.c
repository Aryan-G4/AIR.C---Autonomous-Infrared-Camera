/**
   @copyright (C) 2017 Melexis N.V.
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

// got this file from https://github.com/netzbasteln/MLX90640-Thermocam/blob/master/MLX90640_I2C_Driver.cpp 
// MODIFIED IT: does not require the Arduino library anymore

//#include<Arduino.h>
//#include <Wire.h>

#include "driver/i2c.h"
#include "main.h"
#include "MLX90640_I2C_Driver.h"

void MLX90640_I2CInit()
{
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


//Read a number of words from startAddress. Store into Data array.
//Returns 0 if successful, -1 if error
int MLX90640_I2CRead(uint8_t _deviceAddress, uint16_t startAddress, uint16_t nWordsRead, uint16_t *data)
{
  uint8_t sa;                                                        
  uint8_t command[2] = {0,0};

  sa = (_deviceAddress << 1); // shift 1 bit to left to make space for read/write bit
  command[0] = startAddress >> 8;
  command[1] = startAddress & 0x00FF;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  assert(cmd != NULL);

  // is now using the esp32 i2c commands rather than arduino
  // follows the proper sequence of steps in gathering data
  i2c_master_start(cmd);
  //slave address
  i2c_master_write_byte(cmd,sa | I2C_MASTER_WRITE, true);
  //device registers to be read
  i2c_master_write(cmd,command,2,true);

  i2c_master_start(cmd);
  //slave address before streaming
  i2c_master_write_byte(cmd,sa | I2C_MASTER_READ, true);
  //read all bytes -1 terminated with ACK
  i2c_master_read(cmd,(uint8_t*)data,2*nWordsRead-1,0x0);
  //read final byte termated with NACK
  i2c_master_read_byte(cmd,(uint8_t*)data+(2*nWordsRead-1),0x1);
  i2c_master_stop(cmd);
  
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(5000));
  i2c_cmd_link_delete(cmd);
  
  // camera module accesses data using big endian (msb first) but our memory uses little endian (lsb first) 
  // we need to switch order around to be little endian -- otherwise memory will understand it as garbage
  for(int i=0;i<nWordsRead*2;i+=2){
          uint8_t temp_data=*((uint8_t *)data+i); // saving MSB
          *((uint8_t *)data+i)=*((uint8_t *)data+i+1); // setting LSB to go first
          *((uint8_t *)data+i+1)=temp_data; // setting MSB to go second
  }
    
  if(ret != ESP_OK)
  {
      printf("I2C READ ERROR %i\n", ret);
  }

  return ret;   
}


//Set I2C Freq, in kHz
//MLX90640_I2CFreqSet(1000) sets frequency to 1MHz
void MLX90640_I2CFreqSet(int freq)
{
  //i2c.frequency(1000 * freq);
  //Wire.setClock((long)1000 * freq);
  i2c_driver_delete(I2C_MASTER_PORT);
  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = (freq*1000)
  };
  i2c_param_config(I2C_MASTER_PORT, &conf);
  i2c_driver_install(I2C_MASTER_PORT, conf.mode, 0, 0, 0);

  
}

//Write two bytes to a two byte address
int MLX90640_I2CWrite(uint8_t _deviceAddress, uint16_t writeAddress, uint16_t data){
      
  uint8_t sa;
  uint8_t command[4] = {0,0,0,0};
  static uint16_t dataCheck;
  

  sa = (_deviceAddress << 1);
  command[0] = writeAddress >> 8;
  command[1] = writeAddress & 0x00FF;
  command[2] = data >> 8;
  command[3] = data & 0x00FF;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd,sa | I2C_MASTER_WRITE, true);
  i2c_master_write(cmd,command,4,true);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(500));
  i2c_cmd_link_delete(cmd);

  if(ret != ESP_OK)
  {
      printf("I2C WRITE ERROR\n");
  }
  
  MLX90640_I2CRead(_deviceAddress,writeAddress,1, &dataCheck);
  if ( dataCheck != data)
  {
//    printf("MLX90640_I2CWrite: The write request didn't stick\n");
//    printf("Written 0x%04x; received 0x%04x\n",data,dataCheck);
    return -2;
  }   
  
  return 0;
}

