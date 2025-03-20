#ifndef MAIN_H_
#define MAIN_H_

#include <cstdarg>
#include <mbed.h>
#include <platform.h>
#include "vl53lmz_api.h"



//extern VL53LMZ_Configuration VL53_Config;

/*
 * @Brief: This function will initialize SPI interface
           Pull the chipselect high
   @Parameter:
        bit = number of SPI bits
        mode = Clock polarity and phase mode
            mode | POL PHA
            -----+--------
              0  |  0   0
              1  |  0   1
              2  |  1   0
              3  |  1   1
        freq = clock rate of the SPI
*/
void SPI_Init();

/*
    @Brief: Set up the VL53LMZ Configuration
    @Parameter:
        *p_dev = Pointer to VL53LMZ Configuration
        *CS_pin = Pointer to Chip-select (Pin D7) class
        freq = spi clock speed
*/
void set_spi(VL53LMZ_Configuration *p_dev, DigitalOut *CS_pin, uint32_t freq);


#endif

