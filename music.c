#include "music.h"
#include "pins.h"
#include <wiringPi.h>
#include <softTone.h>
#include <unistd.h> // usleep
#include <stdio.h>

// 간단한 경쾌한 멜로디 패턴 (G4, A4, B4, C5) 반복
// 주파수 정의
#define C4  261
#define D4  293
#define E4  329
#define F4  349
#define G4  392
#define A4  440
#define B4  494
#define C5  523
#define REST 0

#define SPKR 6

static int melody[] = {G4, A4, B4, C5, REST};
static int melodyCount = sizeof(melody)/sizeof(melody[0]);

static void* music_thread_func(void *arg){
    MusicState *state = (MusicState*)arg;
    softToneCreate(SPKR);

    while(state->running){
        if(state->play_bgm){
            // 라운드에 따른 속도/피치 조절
            if(state->round_num <= 5) {
                state->pitchFactor = 1.0f;
                state->speedFactor = 1.0f;
            } else {
                state->pitchFactor = 1.2f;
                state->speedFactor = 0.7f;
            }

            // 멜로디 재생
            for(int i=0; i<melodyCount && state->running && state->play_bgm; i++){
                int note = melody[i];
                int playNote = (int)(note * state->pitchFactor);
                softToneWrite(SPKR, playNote);
                int delayTime = (int)(280 * state->speedFactor);
                if(playNote == REST){
                    softToneWrite(SPKR, 0);
                }
                delay(delayTime);
            }

            // 멜로디 한 번 재생 후 잠깐 쉬고 반복
            delay(200);
        } else {
            // BGM 정지 상태
            softToneWrite(SPKR, 0);
            delay(100);
        }
    }

    softToneWrite(SPKR, 0);
    return NULL;
}

void music_init(MusicState *state, int speaker_pin){
    (void)speaker_pin; // speaker_pin 사용 안함, PIN_BUZZER 직접 사용
    state->round_num = 1;
    state->running = 1;
    state->play_bgm = 0;
    state->pitchFactor = 1.0f;
    state->speedFactor = 1.0f;

    // wiringPiSetup()는 main에서 이미 했다고 가정
    // softToneCreate는 스레드 내에서

    pthread_create(&state->thread, NULL, music_thread_func, state);
}

void music_start_bgm(MusicState *state){
    state->play_bgm = 1;
}

void music_stop_bgm(MusicState *state){
    state->play_bgm = 0;
    softToneWrite(SPKR,0);
}

void music_update_round(MusicState *state, int round_num){
    state->round_num = round_num;
}

void music_close(MusicState *state){
    state->running = 0;
    pthread_join(state->thread, NULL);
    softToneWrite(SPKR,0);
}
