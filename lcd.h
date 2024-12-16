#ifndef LCD_H
#define LCD_H

#include <stdint.h>

extern int lcd_initialized;
extern int spi_lcd_handle;

#define LCD_WIDTH       128
#define LCD_HEIGHT      160
#define ST7735_SWRESET  0x01
#define ST7735_SLPOUT   0x11
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_MADCTL   0x36
#define ST7735_COLMOD   0x3A

uint16_t rgb_to_565(uint8_t r,uint8_t g,uint8_t b);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_reset();
void lcd_init();
void lcd_set_address_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void lcd_fill_rect(int x,int y,int w,int h,uint16_t color);
void lcd_fill_screen(uint16_t color);

#endif // LCD_H

