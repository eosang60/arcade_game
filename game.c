#include "game.h"
#include "lcd.h"
#include "font5x7.h"
#include "pins.h"
#include "mcp3008.h"
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // usleep 필요

// 외부 전역 변수 참조
extern Player player;
extern Obstacle obstacle;
extern int score;
extern int life;
extern int round_num;
extern int game_over;

// char_index 함수
int char_index(char c){
    int ascii = (int)c;
    if(ascii == ' ') return 0; 
    if(ascii >= 'A' && ascii <= 'Z') return ascii - 64; // 'A'(65)->1
    if(ascii == ':') return 27; // ':'(58)->27
    if(ascii >= '0' && ascii <= '9') return 28 + (ascii - '0'); // '0'(48)->28
    return 0; // 지원 안 하는 문자는 space
}

void draw_char(int x, int y, char c, uint16_t color, uint16_t bg){
    int idx = char_index(c);
    if(idx < 0 || idx > 37) idx = 0;
    for(int col = 0; col < 5; col++){
        unsigned char line = font5x7[idx][col];
        for(int row = 0; row < 7; row++){
            uint16_t pixel = (line & (1<<row)) ? color : bg;
            if(x+col < LCD_WIDTH && y+row < LCD_HEIGHT)
                lcd_fill_rect(x+col, y+row, 1, 1, pixel);
        }
    }
    // space 1px
    lcd_fill_rect(x+5, y, 1, 7, bg);
}

void draw_string(int x, int y, const char *str, uint16_t color, uint16_t bg){
    int cx = x;
    while(*str){
        draw_char(cx, y, *str, color, bg);
        cx += 6;
        str++;
    }
}

void draw_status(){
    char status[32];
    snprintf(status, sizeof(status), "ROUND:%d SCORE:%d", round_num, score);
    draw_string(5, 5, status, rgb_to_565(255,255,255), rgb_to_565(0,0,0));

    const char *life_label = "L:";
    draw_string(5, 15, life_label, rgb_to_565(255,255,255), rgb_to_565(0,0,0));
    for(int i = 0; i < life; i++){
        draw_char(11 + i*6, 15, 'O', rgb_to_565(255,0,0), rgb_to_565(0,0,0));
    }
}

void lcd_draw_player(Player *p, uint16_t color){
    lcd_fill_rect(p->x, p->y, p->width, p->height, color);
}

void lcd_draw_obstacle(Obstacle *o, uint16_t color){
    lcd_fill_rect(o->x, o->y, o->width, o->height, color);
}

void update_leds_based_on_life(){
    gpioWrite(PIN_LED_R, 0);
    gpioWrite(PIN_LED_Y, 0);
    gpioWrite(PIN_LED_G, 0);

    if(life >= 1){
        gpioWrite(PIN_LED_R, 1);
    }
    if(life >= 2){
        gpioWrite(PIN_LED_Y, 1);
    }
    if(life >= 3){
        gpioWrite(PIN_LED_G, 1);
    }
}

void life_decrease_and_check(){
    if(life > 0){
        life--;
        update_leds_based_on_life();
        // 필요하면 여기서 buzzer_beep나 효과음 호출 가능
    }
}

void led_blink_all(int times, int delay_ms){
    for(int i = 0; i < times; i++){
        gpioWrite(PIN_LED_R,1);
        gpioWrite(PIN_LED_Y,1);
        gpioWrite(PIN_LED_G,1);
        usleep(delay_ms * 1000);
        gpioWrite(PIN_LED_R,0);
        gpioWrite(PIN_LED_Y,0);
        gpioWrite(PIN_LED_G,0);
        usleep(delay_ms * 1000);
    }
}

void game_init(){
    player.x = LCD_WIDTH / 2 - 10;
    player.y = LCD_HEIGHT - 20;
    player.width = 20;
    player.height = 10;

    obstacle.width = 20;
    obstacle.height = 10;
    obstacle.x = rand() % (LCD_WIDTH - obstacle.width);
    obstacle.y = 0;
    obstacle.speed = 2;

    score = 0;
    life = 3;
    round_num = 1;
    game_over = 0;

    update_leds_based_on_life();
}

void game_update(){
    int x_val = mcp3008_read_channel(0);
    int dir_x = 0;
    if(x_val < 400) dir_x = -1;
    else if(x_val > 600) dir_x = 1;

    player.x += dir_x * 3;
    if(player.x < 0) player.x = 0;
    if(player.x + player.width > LCD_WIDTH) player.x = LCD_WIDTH - player.width;

    round_num = score / 2 + 1;
    int difficulty = round_num;
    obstacle.speed = 2 * difficulty;

    obstacle.y += obstacle.speed;
    if(obstacle.y > LCD_HEIGHT){
        obstacle.y = 0;
        obstacle.x = rand() % (LCD_WIDTH - obstacle.width);
        score++;
        if(score >= 10){
            game_over = 1;
        }
    }

    // 충돌 체크
    if(!(player.x + player.width < obstacle.x || player.x > obstacle.x + obstacle.width ||
         player.y + player.height < obstacle.y || player.y > obstacle.y + obstacle.height)){
        life_decrease_and_check();
        obstacle.y = 0;
        obstacle.x = rand() % (LCD_WIDTH - obstacle.width);
    }

    if(life <= 0){
        game_over = 1;
    }
}

void game_draw(){
    lcd_fill_screen(rgb_to_565(0,0,0));
    lcd_draw_player(&player, rgb_to_565(0,255,0));
    lcd_draw_obstacle(&obstacle, rgb_to_565(255,0,0));
    draw_status();
}
