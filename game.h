#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef struct {
    int x, y;
    int width, height;
} Player;

typedef struct {
    int x, y;
    int width, height;
    int speed;
} Obstacle;

void game_init();
void game_update();
void game_draw();
void draw_string(int x, int y, const char *str, uint16_t color, uint16_t bg);
void draw_status();
void update_leds_based_on_life();
void life_decrease_and_check();
void led_blink_all(int times,int delay_ms);

#endif // GAME_H

