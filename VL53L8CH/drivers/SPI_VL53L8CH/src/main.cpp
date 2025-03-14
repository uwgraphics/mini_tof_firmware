#include "main.h"
#include <cstdio>
#include <cstdlib>
#include "example12.h"

// Define SPI pins and CS pin
//Nucleo F411-RE
SPI spi_device(D11, D12, D13, NC); // MOSI MISO SCLK
SPI *spi_device_ptr = &spi_device;

DigitalOut CS(A0);
DigitalOut *cs_ptr = &CS;

DigitalOut rst_n(D14);
DigitalOut *rst_n_ptr = &rst_n;

// Black Pill
DigitalOut led(PC_13);


BufferedSerial pc(PB_6, PB_7, 921600); // TX and RX 
FileHandle *mbed::mbed_override_console(int fd)
{
    return &pc;
}

// USBSerial serial(false, 0x1f00, 0x2012, 0x0001);
// FileHandle *mbed_target_override_console(int fd)
// {
//     return &serial;
// }


// SPI spi_device(PB_15, PB_14, PB_13, NC); // MOSI MISO SCLK
// SPI *spi_device_ptr = &spi_device;

// DigitalOut CS(PA_8);
// DigitalOut *cs_ptr = &CS;

// DigitalOut rst_n(D14);
// DigitalOut *rst_n_ptr = &rst_n;

DigitalOut test(PB_10);
DigitalOut test2(PB_0);

VL53LMZ_Configuration VL53_Config = {0}; 

uint32_t freq = 3000000;

extern int example12(VL53LMZ_Configuration *p_dev);

// main() runs in its own thread in the OS
int main()
{
    CS = 1;
    rst_n = 1;
    //NSS = 1;
    test = 0;
    test2 = 0;

    pc.set_format(8, mbed::BufferedSerial::Odd, 1);
    // Enter data to VL53LMZ_Configuration
    set_spi(&VL53_Config,cs_ptr, freq);

    // Initialize the SPI
    SPI_Init();

    while (true) {
            printf("Start running Example12: cnh_data\r\n");

            example12(&VL53_Config);

            printf("Done\r\n");
        }
}

void SPI_Init()
{
    // 8 bit word size
    // VL53L8CH SPI mode supported are clock polarity = 1 and clock phase = 1
    spi_device.format(8, 3);

    // Set Frequency
    spi_device.frequency(freq);
}

void set_spi(VL53LMZ_Configuration *p_dev, DigitalOut *CS_pin, uint32_t freq)
{
    p_dev->platform.device = spi_device_ptr;
    p_dev->platform.freq = freq;
    p_dev->platform.CS = cs_ptr;
    p_dev->platform.rst_n = rst_n_ptr;
    p_dev->platform.address = VL53LMZ_DEFAULT_I2C_ADDRESS;
}

