#include "mcp3008.h"
#include <pigpio.h>
#include <stdint.h>

extern int spi_adc_handle;

int mcp3008_read_channel(int channel){
    if(channel < 0 || channel > 7) return -1;
    uint8_t tx[3], rx[3];
    tx[0] = 0x01;
    tx[1] = 0x80 | (channel << 4);
    tx[2] = 0x00;
    spiXfer(spi_adc_handle, (char*)tx, (char*)rx, 3);
    int value = ((rx[1] & 0x03) << 8) | rx[2];
    return value;
}
