#ifndef __AR_GAME_BEE_H__
#define __AR_GAME_BEE_H__

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

#define NUM_BEE                 (1)

#define SIZE_BITMAP_BEE_X       (20)
#define SIZE_BITMAP_BEE_Y       (10)

enum {
    AR_GAME_BEE_ACTION_IMAGE_1 = 1,
    AR_GAME_BEE_ACTION_IMAGE_2,
    AR_GAME_BEE_ACTION_IMAGE_3,
    AR_GAME_BEE_ACTION_IMAGE_END
};

typedef struct {
    bool visible;
    uint32_t x;
    uint32_t y;
    int16_t base_y;       
    uint8_t wave_index;
    uint8_t wave_speed;      
    uint8_t wave_counter;    
    uint8_t action_image;
    uint16_t spawn_delay;
    bool show_minus20;
    int16_t minus20_x;
    int16_t minus20_y;
    uint8_t minus20_timer; 
} ar_game_bee_t;

extern ar_game_bee_t bee[NUM_BEE];

void ar_game_bee_handle(ak_msg_t *msg);

#endif // __AR_GAME_BEE_H__