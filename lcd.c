#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pigpio.h>
#include <unistd.h>
#include "lcd.h"
#include "pins.h"

int lcd_initialized = 0;
int spi_lcd_handle;

uint16_t rgb_to_565(uint8_t r,uint8_t g,uint8_t b){
    return ((r & 0xF8)<<8)|((g & 0xFC)<<3)|(b>>3);
}

void lcd_command(uint8_t cmd){
    gpioWrite(PIN_LCD_DC, 0);
    spiWrite(spi_lcd_handle, (char*)&cmd, 1);
}

void lcd_data(uint8_t data){
    gpioWrite(PIN_LCD_DC, 1);
    spiWrite(spi_lcd_handle, (char*)&data, 1);
}

void lcd_reset(){
    gpioWrite(PIN_LCD_RST, 0); usleep(100000);
    gpioWrite(PIN_LCD_RST, 1); usleep(100000);
}

void lcd_init(){
    lcd_reset();
    lcd_command(ST7735_SWRESET); usleep(150000);
    lcd_command(ST7735_SLPOUT); usleep(150000);
    lcd_command(ST7735_COLMOD); lcd_data(0x05);
    lcd_command(ST7735_MADCTL); lcd_data(0x00);
    lcd_command(ST7735_DISPON); usleep(100000);
    lcd_initialized = 1;
}

void lcd_set_address_window(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1){
    lcd_command(ST7735_CASET);
    lcd_data(0x00); lcd_data(x0);
    lcd_data(0x00); lcd_data(x1);
    lcd_command(ST7735_RASET);
    lcd_data(0x00); lcd_data(y0);
    lcd_data(0x00); lcd_data(y1);
    lcd_command(ST7735_RAMWR);
}

void lcd_fill_rect(int x,int y,int w,int h,uint16_t color){
    if(!lcd_initialized) return;
    if(x < 0) { w += x; x = 0; }
    if(y < 0) { h += y; y = 0; }
    if((x + w) > LCD_WIDTH) w = LCD_WIDTH - x;
    if((y + h) > LCD_HEIGHT) h = LCD_HEIGHT - y;
    if(w <= 0 || h <= 0) return;

    lcd_set_address_window(x, y, x + w - 1, y + h - 1);
    gpioWrite(PIN_LCD_DC, 1);
    int size = w * h;
    char *buf = (char*)malloc(size * 2);
    if(!buf){
        fprintf(stderr, "Memory allocation failed in lcd_fill_rect\n");
        return;
    }
    for(int i = 0; i < size; i++){
        buf[i*2] = color >> 8;
        buf[i*2+1] = color & 0xFF;
    }
    spiWrite(spi_lcd_handle, buf, size * 2);
    free(buf);
}

void lcd_fill_screen(uint16_t color){
    lcd_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}
