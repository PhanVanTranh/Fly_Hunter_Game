#include "ar_game_boss.h"
#include "ar_game_arrow.h"
#include "ar_game_border.h"
#include "scr_fly_hunter_game.h"

#define BOSS_MOVE_DELAY         2
#define BOSS_HP_INCREMENT       5
#define BOSS_SCORE_REWARD       200
#define NEXT_BOSS_SCORE_STEP    1000
#define BOSS_EXPLODE_TIME       24

ar_game_boss_t boss;

/* Boss mạnh dần sau mỗi lần bị tiêu diệt */
static uint8_t boss_hp = 10;

/* Boss di chuyển chậm */
static uint8_t boss_move_tick = 0;

/****************************************************/
/* Collision                                        */
/****************************************************/

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
/*--------------------------------------------------*/

void ar_game_boss_handle(ak_msg_t *msg)
{
    switch(msg->sig)
    {
    /**************************************************/
    /* Boss Setup                                     */
    /**************************************************/

    case AR_GAME_BOSS_SETUP:
    {
        boss.visible = BLACK;
        boss.state = BOSS_STATE_HIDE;

        boss.hp_max = boss_hp;
        boss.hp = boss_hp;
        boss.display_hp = boss_hp;

        boss.hit_flash = 0;

        boss.shake_timer = 0;
        boss.shake_offset = 0;

        boss.explode_frame = 0;
        boss.explode_timer = 0;

        boss.x = 128;
        boss.y = 15;

        boss_move_tick = 0;
    }
    break;


    /**************************************************/
    /* Boss Spawn                                     */
    /**************************************************/

    case AR_GAME_BOSS_SPAWN:
    {
        APP_DBG_SIG("AR_GAME_BOSS_SPAWN\n");

        /* Xuất hiện Boss */

        boss.visible = WHITE;
        boss.state = BOSS_STATE_ALIVE;

        /* Reset HP */

        boss.hp_max = boss_hp;
        boss.hp = boss_hp;
        boss.display_hp = boss_hp;

        /* Reset animation */

        boss.hit_flash = 0;

        boss.shake_timer = 0;
        boss.shake_offset = 0;

        boss.explode_frame = 0;
        boss.explode_timer = 0;

        /* Spawn */

        boss.x = 128;
        boss.y = 15;

        boss_move_tick = 0;

        /**************************************************
            Xóa toàn bộ Fly
        **************************************************/

        task_post_pure_msg(
            AR_GAME_METEOROID_ID,
            AR_GAME_METEOROID_RESET);

        /**************************************************
            Xóa Bee
        **************************************************/

        task_post_pure_msg(
            AR_GAME_BEE_ID,
            AR_GAME_BEE_RESET);

        /**************************************************
            Xóa toàn bộ Arrow đang bay
        **************************************************/

        for(uint8_t i=0;i<MAX_NUM_ARROW;i++)
        {
            arrow[i].visible = BLACK;
            arrow[i].x = 0;
            arrow[i].y = 0;
        }

        /**************************************************
            Trả lại full tên
        **************************************************/

        settingsetup.num_arrow = MAX_NUM_ARROW;

        fly_hunter.action_image =
            AR_GAME_FLY_HUNTER_ACTION_IMAGE_1;

        /**************************************************
            Âm thanh Boss
        **************************************************/

        BUZZER_PlaySound(BUZZER_SOUND_3BEEP);
    }
    break;
    

    
    /**************************************************/
    /* Boss Run                                       */
    /**************************************************/

    case AR_GAME_BOSS_RUN:
    {
        if(!boss.visible)
            break;

        /**************************************************
            Boss đang nổ
        **************************************************/

        if(boss.state == BOSS_STATE_DYING)
        {
            if(boss.explode_timer > 0)
            {
                boss.explode_timer--;

                if((boss.explode_timer % 4) == 0)
                {
                    boss.explode_frame++;
                }
            }
            else
            {
                /**********************************************
                    Kết thúc animation nổ
                **********************************************/

                boss.visible = BLACK;

                boss.state = BOSS_STATE_HIDE;

                game_state = GAME_STATE_NORMAL;

                ar_game_score += BOSS_SCORE_REWARD;

                next_boss_score += NEXT_BOSS_SCORE_STEP;

                boss_hp += BOSS_HP_INCREMENT;

                /**********************************************
                    Spawn lại Fly
                **********************************************/

                task_post_pure_msg(
                    AR_GAME_METEOROID_ID,
                    AR_GAME_METEOROID_SETUP);

                /**********************************************
                    Spawn lại Bee
                **********************************************/

                task_post_pure_msg(
                    AR_GAME_BEE_ID,
                    AR_GAME_BEE_SETUP);
            }

            break;
        }

        /**************************************************
            Chỉ chạy khi Boss Mode
        **************************************************/

        if(game_state != GAME_STATE_BOSS)
            break;

        /**************************************************
            Boss di chuyển chậm
        **************************************************/

        boss_move_tick++;

        if(boss_move_tick >= BOSS_MOVE_DELAY)
        {
            boss_move_tick = 0;

            boss.x--;
        }

        /**************************************************
            Boss rung khi trúng đạn
        **************************************************/

        if(boss.shake_timer > 0)
        {
            boss.shake_timer--;

            if(boss.shake_timer & 1)
                boss.shake_offset = -1;
            else
                boss.shake_offset = 1;
        }
        else
        {
            boss.shake_offset = 0;
        }

        /**************************************************
            Boss nhấp nháy
        **************************************************/

        if(boss.hit_flash > 0)
        {
            boss.hit_flash--;
        }

        /**************************************************
            Thanh HP giảm mượt
        **************************************************/

        if(boss.display_hp > boss.hp)
        {
            boss.display_hp--;
        }

        /**************************************************
            Boss chạm Border
        **************************************************/

        if(boss.x <= AXIS_X_BORDER)
        {
            boss.visible = BLACK;

            boss.state = BOSS_STATE_HIDE;

            if(player_life > 0)
            {
                player_life--;
            }

            if(player_life == 0)
            {
                task_post_pure_msg(
                    AC_TASK_DISPLAY_ID,
                    AR_GAME_RESET);
            }
            else
            {
                game_state = GAME_STATE_NORMAL;

                next_boss_score += 500;

                /**********************************************
                    Spawn lại Fly
                **********************************************/

                task_post_pure_msg(
                    AR_GAME_METEOROID_ID,
                    AR_GAME_METEOROID_SETUP);

                /**********************************************
                    Spawn lại Bee
                **********************************************/

                task_post_pure_msg(
                    AR_GAME_BEE_ID,
                    AR_GAME_BEE_SETUP);
            }
        }

    }
    break;
    /**************************************************/
    /* Boss Detonator                                 */
    /**************************************************/

    case AR_GAME_BOSS_DETONATOR:
    {
        if(!boss.visible)
            break;

        if(boss.state != BOSS_STATE_ALIVE)
            break;

        for(uint8_t i = 0; i < MAX_NUM_ARROW; i++)
        {
            if(arrow[i].visible != WHITE)
                continue;

            if(IS_COLLIDE(

                arrow[i].x,
                arrow[i].y,
                SIZE_BITMAP_ARROW_X,
                SIZE_BITMAP_ARROW_Y,

                boss.x,
                boss.y,
                SIZE_BITMAP_BOSS_X,
                SIZE_BITMAP_BOSS_Y))
            {
                /********************************************
                    Xóa mũi tên
                ********************************************/

                arrow[i].visible = BLACK;
                arrow[i].x = 0;
                arrow[i].y = 0;

                if(settingsetup.num_arrow < MAX_NUM_ARROW)
                {
                    settingsetup.num_arrow++;

                    fly_hunter.action_image =
                        AR_GAME_FLY_HUNTER_ACTION_IMAGE_1;
                }

                /********************************************
                    Boss bị trúng đạn
                ********************************************/

                if(boss.hp > 0)
                {
                    boss.hp--;

                    boss.hit_flash = 6;

                    boss.shake_timer = 8;

                    BUZZER_PlaySound(BUZZER_SOUND_BANG);
                }

                /********************************************
                    Boss chết
                ********************************************/

                if(boss.hp == 0)
                {
                    boss.state = BOSS_STATE_DYING;

                    boss.explode_frame = 0;

                    boss.explode_timer = BOSS_EXPLODE_TIME;

                    boss.hit_flash = 0;

                    boss.shake_timer = 0;

                    boss.display_hp = 0;

                    BUZZER_PlaySound(BUZZER_SOUND_BANG);
                }

                break;
            }
        }
    }
    break;
    /**************************************************/
    /* Boss Reset                                     */
    /**************************************************/

    case AR_GAME_BOSS_RESET:
    {
        boss.visible = BLACK;

        boss.state = BOSS_STATE_HIDE;

        boss.hp = boss_hp;

        boss.display_hp = boss_hp;

        boss.hit_flash = 0;

        boss.shake_timer = 0;

        boss.shake_offset = 0;

        boss.explode_frame = 0;

        boss.explode_timer = 0;

        boss.x = 128;

        boss.y = 15;

        boss_move_tick = 0;
    }
    break;
    default:
        break;
    }
}

/************************************************************/
/* Boss Display                                             */
/************************************************************/

void ar_game_boss_display()
{
    /**************************************************
        Không hiển thị
    **************************************************/

    if(!boss.visible)
        return;

    /**************************************************
        Animation nổ
    **************************************************/

    if(boss.state == BOSS_STATE_DYING)
    {
        switch(boss.explode_frame % 3)
        {
            case 0:
            {
                view_render.drawBitmap(
                    boss.x,
                    boss.y,
                    bitmap_bang_I,
                    SIZE_BITMAP_BANG_I_X,
                    SIZE_BITMAP_BANG_I_Y,
                    WHITE);
            }
            break;

            case 1:
            {
                view_render.drawBitmap(
                    boss.x - 2,
                    boss.y - 2,
                    bitmap_bang_II,
                    SIZE_BITMAP_BANG_I_X,
                    SIZE_BITMAP_BANG_I_Y,
                    WHITE);
            }
            break;

            case 2:
            {
                view_render.drawBitmap(
                    boss.x - 4,
                    boss.y - 4,
                    bitmap_bang_III,
                    SIZE_BITMAP_BANG_II_X,
                    SIZE_BITMAP_BANG_II_Y,
                    WHITE);
            }
            break;
        }

        return;
    }

    /**************************************************
        Boss bị bắn -> nhấp nháy
    **************************************************/

    if(boss.hit_flash)
    {
        if(boss.hit_flash & 1)
        {
            return;
        }
    }

    /**************************************************
        Vẽ Boss
    **************************************************/

    view_render.drawBitmap(

        boss.x + boss.shake_offset,

        boss.y,

        bitmap_boss_fly,

        SIZE_BITMAP_BOSS_X,

        SIZE_BITMAP_BOSS_Y,

        WHITE);

    /**************************************************
        HP Bar
    **************************************************/

    uint8_t hp_y;

    if(boss.y < 5)
    {
        hp_y = boss.y + SIZE_BITMAP_BOSS_Y + 2;
    }
    else
    {
        hp_y = boss.y - 4;
    }

    view_render.drawRect(

        boss.x + boss.shake_offset,

        hp_y,

        24,

        3,

        WHITE);

    uint8_t hp_width =
        (boss.display_hp * 22) / boss.hp_max;

    view_render.fillRect(

        boss.x + boss.shake_offset + 1,

        hp_y + 1,

        hp_width,

        1,

        WHITE);

    /**************************************************
        Hiển thị số HP
    **************************************************/

// Nếu OLED còn chỗ thì bỏ comment

/*
    view_render.setTextSize(1);

    view_render.setCursor(
        boss.x,
        hp_y - 8);

    view_render.print(boss.hp);
*/
}