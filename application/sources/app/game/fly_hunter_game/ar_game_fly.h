#ifndef __AR_GAME_FLY_H__
#define __AR_GAME_FLY_H__

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


#define NUM_FLYs              (3)
#define SIZE_BITMAP_FLYs_X    (20)
#define SIZE_BITMAP_FLYs_Y    (20)

#define RANDOM_FLY_X()          ((rand() % 39) + 130)
#define RANDOM_FLY_Y_TOP()      (rand() % 4)          
#define RANDOM_FLY_Y_MIDDLE()   (18 + rand() % 4)    
#define RANDOM_FLY_Y_BOTTOM()   (36 + rand() % 4)    

enum {
    AR_GAME_FLY_ACTION_IMAGE_1 = 1,
    AR_GAME_FLY_ACTION_IMAGE_2,
    AR_GAME_FLY_ACTION_IMAGE_3,
    AR_GAME_FLY_ACTION_IMAGE_END
};

typedef struct {
    bool visible;
    uint32_t x, y;
    uint8_t action_image;
} ar_game_fly_t;

extern ar_game_fly_t fly[NUM_FLYs];
void fly_random_position(uint8_t index);
#endif //__AR_GAME_FLY_H__
