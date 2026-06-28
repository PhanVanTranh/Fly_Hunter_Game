#ifndef __AR_GAME_METEOROID_H__
#define __AR_GAME_METEOROID_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
#include "scr_fly_hunter_game.h"

// #define NUM_METEOROIDS              (2)
// #define SIZE_BITMAP_METEOROIDS_X    (40)
// #define SIZE_BITMAP_METEOROIDS_Y    (30)

#define NUM_METEOROIDS              (3)
#define SIZE_BITMAP_METEOROIDS_X    (25)
#define SIZE_BITMAP_METEOROIDS_Y    (20)

// #define AXIS_Y_METEOROID_START   (2)
// #define AXIS_Y_METEOROID_STEP    (12)

//#define RANDOM_METEOROID_Y() (rand() % 34)

#define RANDOM_METEOROID_X() ((rand() % 39) + 130)
#define RANDOM_METEOROID_Y_TOP()      (rand() % 4)          // 0~3
#define RANDOM_METEOROID_Y_MIDDLE()   (18 + rand() % 4)    // 18~21
#define RANDOM_METEOROID_Y_BOTTOM()   (36 + rand() % 4)    // 36~39

enum {
    AR_GAME_METEOROID_ACTION_IMAGE_1 = 1,
    AR_GAME_METEOROID_ACTION_IMAGE_2,
    AR_GAME_METEOROID_ACTION_IMAGE_3,
    AR_GAME_METEOROID_ACTION_IMAGE_END
};

typedef struct {
    bool visible;
    uint32_t x, y;
    uint8_t action_image;
} ar_game_meteoroid_t;

extern ar_game_meteoroid_t meteoroid[NUM_METEOROIDS];
void meteoroid_adjust_position(uint8_t index);
void meteoroid_random_position(uint8_t index);
#endif //__AR_GAME_METEOROID_H__
