#ifndef __AR_GAME_BOSS_H__
#define __AR_GAME_BOSS_H__

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

/*=========================================================
    Boss Config
=========================================================*/

#define NUM_BOSS                   (1)

#define SIZE_BITMAP_BOSS_X         (35)
#define SIZE_BITMAP_BOSS_Y         (35)

/*=========================================================
    Animation
=========================================================*/

enum
{
    AR_GAME_BOSS_ACTION_IMAGE_1 = 1,
    AR_GAME_BOSS_ACTION_IMAGE_2,
    AR_GAME_BOSS_ACTION_IMAGE_3,

    AR_GAME_BOSS_ACTION_IMAGE_END
};

/*=========================================================
    Boss Object
=========================================================*/

typedef struct
{
    uint8_t x;
    uint8_t y;

    uint8_t visible;

    uint8_t hp;
    uint8_t hp_max;

    uint8_t display_hp;

    uint8_t hit_flash;

    uint8_t shake_timer;
    int8_t  shake_offset;

    uint8_t state;
    uint8_t explode_frame;
    uint8_t explode_timer;

} ar_game_boss_t;


enum
{
    BOSS_STATE_HIDE = 0,
    BOSS_STATE_ALIVE,
    BOSS_STATE_DYING
};

/*=========================================================
    Global
=========================================================*/

extern ar_game_boss_t boss;

/*=========================================================
    Function
=========================================================*/

void ar_game_boss_handle(ak_msg_t *msg);

void boss_spawn(void);

void boss_reset(void);
void ar_game_boss_display(void);
#endif
