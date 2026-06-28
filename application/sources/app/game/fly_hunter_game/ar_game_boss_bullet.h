#ifndef __AR_GAME_BOSS_BULLET_H__
#define __AR_GAME_BOSS_BULLET_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_display.h"

#include "scr_fly_hunter_game.h"
#include "ar_game_boss.h"
#include "ar_game_fly_hunter.h"

/**************************************************/

#define NUM_BOSS_BULLET             4

#define SIZE_BITMAP_BOSS_BULLET_X   4
#define SIZE_BITMAP_BOSS_BULLET_Y   4

/**************************************************/

typedef struct
{
    int16_t x;
    int16_t y;

    int8_t vx;
    int8_t vy;

    uint8_t visible;

} ar_game_boss_bullet_t;

/**************************************************/

extern ar_game_boss_bullet_t boss_bullet[NUM_BOSS_BULLET];

/**************************************************/

void ar_game_boss_bullet_handle(ak_msg_t *msg);

void ar_game_boss_bullet_display(void);

#endif