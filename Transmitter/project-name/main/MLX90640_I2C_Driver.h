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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MLX90640_I2C_Driver_H_
#define _MLX90640_I2C_Driver_H_

#include <stdint.h>

//Define the size of the I2C buffer based on the platform the user has
#define I2C_BUFFER_LENGTH 32


void MLX90640_I2CInit(void);
int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nWordsRead, uint16_t *data);
int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data);
void MLX90640_I2CFreqSet(int freq);
#endif

#ifdef __cplusplus
}
#endif


/*
#ifndef _REDAR_I2C_DRIVER_
#define _REDAR_I2C_DRIVER_


#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_BUS_FREQUENCY_HZ 1000000
#define ACK_CHECK_EN 0x1            
#define ACK_CHECK_DIS 0x0          
#define ACK_VAL 0x0               
#define NACK_VAL 0x1 
#define I2C_MASTER_TIMEOUT_MS 1000

#include <stdint.h>


    void REDAR_I2CInit(void);
    int REDAR_I2CRead(uint8_t slaveAddr,uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);
    int REDAR_I2CWrite(uint8_t slaveAddr,uint16_t writeAddress, uint16_t data);
#endif

*/