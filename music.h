#ifndef MUSIC_H
#define MUSIC_H

#include <pthread.h>

// 음악 상태 구조체
typedef struct {
    int round_num;
    int running;
    int play_bgm;      // BGM 재생 여부
    float pitchFactor;
    float speedFactor;
    pthread_t thread;
} MusicState;

void music_init(MusicState *state, int speaker_pin);
void music_start_bgm(MusicState *state);
void music_stop_bgm(MusicState *state);
void music_update_round(MusicState *state, int round_num);
void music_close(MusicState *state);

#endif // MUSIC_H
