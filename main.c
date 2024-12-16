#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h> 
#include <string.h>
#include <pigpio.h>
#include <time.h>
#include <wiringPi.h>
#include <softTone.h>
#include "pins.h"
#include "lcd.h"
#include "mcp3008.h"
#include "game.h"
#include "music.h"

// 전역변수
Player player;
Obstacle obstacle;
int score = 0;
int life = 3;
int round_num = 1;
int game_over = 0;

int spi_adc_handle;
extern int lcd_initialized;
extern int spi_lcd_handle;

int main(int argc,char*argv[]){
    wiringPiSetupGpio();

    if(gpioInitialise() < 0){
        fprintf(stderr,"pigpio init failed\n");
        return -1;
    }

    pinMode(PIN_BUZZER, OUTPUT);

    srand(time(NULL));

    gpioSetMode(PIN_LED_R, PI_OUTPUT); gpioWrite(PIN_LED_R, 0);
    gpioSetMode(PIN_LED_Y, PI_OUTPUT); gpioWrite(PIN_LED_Y, 0);
    gpioSetMode(PIN_LED_G, PI_OUTPUT); gpioWrite(PIN_LED_G, 0);

    gpioSetMode(PIN_SWITCH, PI_INPUT);
    gpioSetPullUpDown(PIN_SWITCH, PI_PUD_UP);

    spi_lcd_handle = spiOpen(0, 32000000, 0);
    if(spi_lcd_handle < 0){
        fprintf(stderr,"LCD SPI open fail\n");
        gpioTerminate();
        return -1;
    }

    spi_adc_handle = spiOpen(1, 1350000, 0);
    if(spi_adc_handle < 0){
        fprintf(stderr,"MCP3008 SPI open fail\n");
        spiClose(spi_lcd_handle);
        gpioTerminate();
        return -1;
    }

    gpioSetMode(PIN_LCD_DC, PI_OUTPUT);
    gpioSetMode(PIN_LCD_RST, PI_OUTPUT);

    lcd_init();
    lcd_fill_screen(rgb_to_565(0,0,0));

    game_init();
    update_leds_based_on_life();

    // MusicState 관리
    MusicState music_state;
    music_init(&music_state, PIN_BUZZER);
    music_start_bgm(&music_state); // 게임 시작시 BGM 재생 시작

    while(1){
        while(!game_over){
            game_update();
            game_draw();

            // round_num 바뀌면 music_update_round 호출
            music_update_round(&music_state, round_num);

            usleep(33000);
        }

        // 게임 오버 후
        music_stop_bgm(&music_state); // BGM 중지

        lcd_fill_screen(rgb_to_565(0,0,0));
        if(score >= 10){
            const char *message1 = "GAME CLEAR";
            int text_length1 = strlen(message1);
            int text_width1 = text_length1 * 6; 
            int text_x1 = (LCD_WIDTH - text_width1) / 2;
            int text_y1 = 60;
            draw_string(text_x1, text_y1, message1, rgb_to_565(255,255,255), rgb_to_565(0,0,0));

            // 여기서 승리 사운드 내고 싶다면 별도 함수를 music.c에 추가해 호출 가능
        } else {
            const char *message1 = "GAME OVER";
            int text_length1 = strlen(message1);
            int text_width1 = text_length1 * 6;
            int text_x1 = (LCD_WIDTH - text_width1) / 2;
            int text_y1 = 60;
            draw_string(text_x1, text_y1, message1, rgb_to_565(255,0,0), rgb_to_565(0,0,0));

            // 게임 오버 사운드도 필요하면 넣을 수 있음, 지금은 생략
        }

        const char *restart_msg = "PLEASE RESTART";
        int restart_length = strlen(restart_msg);
        int restart_width = restart_length * 6;
        int restart_x = (LCD_WIDTH - restart_width) / 2;
        int restart_y = 80;
        draw_string(restart_x, restart_y, restart_msg, rgb_to_565(255,255,255), rgb_to_565(0,0,0));

        led_blink_all(3, 200);
        gpioWrite(PIN_LED_R,0);
        gpioWrite(PIN_LED_Y,0);
        gpioWrite(PIN_LED_G,0);

        while(1){
            if(gpioRead(PIN_SWITCH) == 0){
                usleep(20000);
                if(gpioRead(PIN_SWITCH) == 0){
                    game_init();
                    update_leds_based_on_life();
                    game_over = 0;
                    // 게임 재시작 시 BGM 다시 start
                    music_start_bgm(&music_state);
                    break;
                }
            }
            usleep(10000);
        }
    }

    // 게임 종료 시
    music_close(&music_state);
    spiClose(spi_adc_handle);
    spiClose(spi_lcd_handle);
    gpioTerminate();
    return 0;
}
