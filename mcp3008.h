#ifndef MCP3008_H
#define MCP3008_H

// spi_adc_handle를 extern 선언
extern int spi_adc_handle;
int mcp3008_read_channel(int channel);

#endif
