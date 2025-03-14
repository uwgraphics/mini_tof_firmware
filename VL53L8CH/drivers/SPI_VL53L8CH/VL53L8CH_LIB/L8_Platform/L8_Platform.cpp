/*******************************************************************************
* Copyright (c) 2020, STMicroelectronics - All Rights Reserved
*
* This file is part of the VL53LMZ Ultra Lite Driver and is dual licensed,
* either 'STMicroelectronics Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
********************************************************************************
*
* 'STMicroelectronics Proprietary license'
*
********************************************************************************
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*
********************************************************************************
*
* Alternatively, the VL53LMZ Ultra Lite Driver may be distributed under the
* terms of 'BSD 3-clause "New" or "Revised" License', in which case the
* following provisions apply instead of the ones mentioned above :
*
********************************************************************************
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
*******************************************************************************/


#include "L8_Platform.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define VL53L8CX_COMMS_CHUNK_SIZE 3096
#define SPI_WRITE_MASK(x) (uint16_t)(x | 0x8000)
#define SPI_READ_MASK(x) (uint16_t)(x & ~0x8000)

volatile bool transfer_complete = false;

// Callback function called when transfer is complete
void transfer_callback(int event) {
    if (event == SPI_EVENT_COMPLETE) {
        transfer_complete = true;
    }
}

uint8_t RdByte(
		VL53LMZ_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value)
{
	uint8_t status = 255;
	
	/* Need to be implemented by customer. This function returns 0 if OK */
    status = RdMulti(p_platform, RegisterAdress, p_value, 1);
	return status;
}

uint8_t WrByte(
		VL53LMZ_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value)
{
	uint8_t status = 255;

	/* Need to be implemented by customer. This function returns 0 if OK */
    status = WrMulti(p_platform, RegisterAdress, &value, 1);
	return status;
}

uint8_t WrMulti(
		VL53LMZ_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status = 255;
    uint32_t i = 0;
    uint32_t position = 0;
    uint32_t data_size = 0;
    uint16_t temp;
    uint8_t data_write[VL53L8CX_COMMS_CHUNK_SIZE + 2];
    uint8_t data_read[VL53L8CX_COMMS_CHUNK_SIZE + 2];
    uint32_t byte_count;
	
	/* Need to be implemented by customer. This function returns 0 if OK */
    //printf("WrMulti -> Address: %x\r\n", RegisterAdress);

    for (position = 0; position < size; position += VL53L8CX_COMMS_CHUNK_SIZE)
    {
        if (size > VL53L8CX_COMMS_CHUNK_SIZE)
        {
            if ((position + VL53L8CX_COMMS_CHUNK_SIZE) > size)
            {
            data_size = size - position;
            }
            else
            {
            data_size = VL53L8CX_COMMS_CHUNK_SIZE;
            }
        }
        else
        {
            data_size = size;
        }
    
        temp = RegisterAdress + position;

        // Set the first byte: Write bit + upper 7 bits of the register address
        data_write[0] = SPI_WRITE_MASK(temp) >> 8;
        data_write[1] = SPI_WRITE_MASK(temp) & 0xFF;

        for (i = 0; i < data_size; i++)
        {
            data_write[i+2] = p_values[position+i];
        }

        data_size += 2;

        p_platform->CS->write(0);
        byte_count = p_platform->device->write((char *)data_write, data_size, (char*)data_read, 0);
        if (byte_count != data_size)
        {
            printf("byte_count not match\r\n");
            return 1;
        }
        p_platform->CS->write(1);
    }
    return 0;
}

uint8_t RdMulti(
		VL53LMZ_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status = 255;
    uint32_t position = 0;
    uint32_t data_size = 0;
    uint16_t temp;
    uint8_t data_write[VL53L8CX_COMMS_CHUNK_SIZE + 2];
    uint8_t data_read[VL53L8CX_COMMS_CHUNK_SIZE + 2];
    uint32_t byte_count;

	// printf("RdMulti -> Address: %x\r\n", RegisterAdress);
    
    for (position = 0; position < size; position += VL53L8CX_COMMS_CHUNK_SIZE)
    {
        if (size > VL53L8CX_COMMS_CHUNK_SIZE)
        {
            if ((position + VL53L8CX_COMMS_CHUNK_SIZE) > size)
            {
            data_size = size - position;
            }
            else
            {
            data_size = VL53L8CX_COMMS_CHUNK_SIZE;
            }
        }
        else
        {
            data_size = size;
        }

        temp = RegisterAdress + position;

        data_write[0] = SPI_READ_MASK(temp) >> 8;
        data_write[1] = SPI_READ_MASK(temp) & 0xFF;
        
        *(p_platform->CS) = 0;
        byte_count = p_platform->device->write((char*)data_write, 2, (char*)data_read, 0);
        if (byte_count != 2)
        {
            printf("byte_count not match\r\n");
            return 1;
        }
        byte_count = p_platform->device->write((char*)data_write, 0, (char*)(p_values + position), data_size);
        if (byte_count != data_size)
        {
            printf("byte_count not match\r\n");
            return 1;
        }
        *(p_platform->CS) = 1;

    }
	return 0;
}

uint8_t Reset_Sensor(
		VL53LMZ_Platform *p_platform)
{
	uint8_t status = 0;
	
	/* (Optional) Need to be implemented by customer. This function returns 0 if OK */
	
	/* Set pin LPN to LOW */
	/* Set pin AVDD to LOW */
	/* Set pin VDDIO  to LOW */
    p_platform->rst_n->write(0);
	WaitMs(p_platform, 100);


	/* Set pin LPN of to HIGH */
	/* Set pin AVDD of to HIGH */
	/* Set pin VDDIO of  to HIGH */
    p_platform->rst_n->write(1);
	WaitMs(p_platform, 100);

	return status;
}

void SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;
	
	/* Example of possible implementation using <string.h> */
	for(i = 0; i < size; i = i + 4) 
	{
		tmp = (
		  buffer[i]<<24)
		|(buffer[i+1]<<16)
		|(buffer[i+2]<<8)
		|(buffer[i+3]);
		
		memcpy(&(buffer[i]), &tmp, 4);
	}
}	

uint8_t WaitMs(
		VL53LMZ_Platform *p_platform,
		uint32_t TimeMs)
{
	uint8_t status = 255;

	/* Need to be implemented by customer. This function returns 0 if OK */
	ThisThread::sleep_for(TimeMs);
	return 0;
}
