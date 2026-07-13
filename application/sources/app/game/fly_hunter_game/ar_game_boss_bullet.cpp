#include "ar_game_boss_bullet.h"

ar_game_boss_bullet_t boss_bullet[NUM_BOSS_BULLET];

static bool IS_COLLIDE(
    int32_t x1,
    int32_t y1,
    int32_t w1,
    int32_t h1,

    int32_t x2,
    int32_t y2,
    int32_t w2,
    int32_t h2)
{
    return

        x1 < (x2 + w2) &&
        (x1 + w1) > x2 &&

        y1 < (y2 + h2) &&
        (y1 + h1) > y2;
}

#define BOSS_BULLET_SPEED_X      (-3)

static uint16_t boss_get_fire_interval()
{
    if(boss.hp <= boss.hp_max / 5)
    {
        return 350;
    }

    if(boss.hp <= boss.hp_max / 2)
    {
        return 500;
    }

    return 700;
}

static void boss_fire_one_bullet()
{
    for(uint8_t i=0;i<NUM_BOSS_BULLET;i++)
    {
        if(boss_bullet[i].visible == BLACK)
        {
            boss_bullet[i].visible = WHITE;

            boss_bullet[i].x =
                boss.x + 4;

            boss_bullet[i].y =
                boss.y + SIZE_BITMAP_BOSS_Y/2 - 2;

            boss_bullet[i].vx = -3;

            int16_t dy =
                fly_hunter.y -
                boss_bullet[i].y;

            if(dy > 8)
                dy = 1;
            else if(dy < -8)
                dy = -1;
            else
                dy = 0;

            boss_bullet[i].vy = dy;

            break;
        }
    }
}

void ar_game_boss_bullet_handle(ak_msg_t *msg)
{
    switch(msg->sig)
    {
    case AR_GAME_BOSS_BULLET_SETUP:
    {
        for(uint8_t i=0;i<NUM_BOSS_BULLET;i++)
        {
            boss_bullet[i].visible = BLACK;

            boss_bullet[i].x = 0;
            boss_bullet[i].y = 0;

            boss_bullet[i].vx = 0;
            boss_bullet[i].vy = 0;
        }
    }
    break;

    case AR_GAME_BOSS_BULLET_FIRE:
    {
        if(!boss.visible)
            break;

        if(game_state != GAME_STATE_BOSS)
            break;

        boss_fire_one_bullet();

        timer_set(
            AR_GAME_BOSS_BULLET_ID,
            AR_GAME_BOSS_BULLET_FIRE,
            boss_get_fire_interval(),
            TIMER_ONE_SHOT);
    }
    break;

        case AR_GAME_BOSS_BULLET_RUN:
    {
        for(uint8_t i=0;i<NUM_BOSS_BULLET;i++)
        {
            if(boss_bullet[i].visible == BLACK)
                continue;

            boss_bullet[i].x += boss_bullet[i].vx;
            boss_bullet[i].y += boss_bullet[i].vy;
                        if(boss_bullet[i].x < 0)
            {
                boss_bullet[i].visible = BLACK;
                continue;
            }

            if(boss_bullet[i].y < 0)
            {
                boss_bullet[i].visible = BLACK;
                continue;
            }

            if(boss_bullet[i].y > 64)
            {
                boss_bullet[i].visible = BLACK;
                continue;
            }

            if(IS_COLLIDE(
                boss_bullet[i].x,
                boss_bullet[i].y,
                SIZE_BITMAP_BOSS_BULLET_X,
                SIZE_BITMAP_BOSS_BULLET_Y,

                fly_hunter.x,
                fly_hunter.y,
                SIZE_BITMAP_FLY_HUNTER_X,
                SIZE_BITMAP_FLY_HUNTER_Y))
            {
                boss_bullet[i].visible = BLACK;

                if(player_life)
                {
                    player_life--;

                    BUZZER_PlaySound(
                        BUZZER_SOUND_BANG);
                }

                if(player_life==0)
                {
                    task_post_pure_msg(
                        AC_TASK_DISPLAY_ID,
                        AR_GAME_RESET);
                }
            }
        }
    }
    break;


    case AR_GAME_BOSS_BULLET_RESET:
    {
        timer_remove_attr(
            AR_GAME_BOSS_BULLET_ID,
            AR_GAME_BOSS_BULLET_FIRE);

        for(uint8_t i=0;i<NUM_BOSS_BULLET;i++)
        {
            boss_bullet[i].visible = BLACK;
        }
    }
    break;

    default:
        break;
    }

}

