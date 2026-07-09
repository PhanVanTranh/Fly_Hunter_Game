#include "ar_game_fly.h"

#include "ar_game_arrow.h"
#include "ar_game_bang.h"
#include "ar_game_border.h"
#include "scr_fly_hunter_game.h"
#include "ar_game_butterfly.h"

#define IS_COLLIDE(x1,y1,w1,h1,x2,y2,w2,h2) (((x1) < ((x2) + (w2))) && \
                                             (((x1) + (w1)) > (x2)) && \
                                             ((y1) < ((y2) + (h2))) && \
                                             (((y1) + (h1)) > (y2)))

#define RANDOM_FLY_ACTION_IMAGE() (rand() % AR_GAME_FLY_ACTION_IMAGE_3 + AR_GAME_FLY_ACTION_IMAGE_1)

#define SAFE_DISTANCE_X     30
#define SAFE_DISTANCE_Y     12
#define MAX_POSITION_ATTEMPTS 20

ar_game_fly_t fly[NUM_FLYs];

void ar_game_fly_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case AR_GAME_FLY_SETUP: {
        APP_DBG_SIG("AR_GAME_FLY_SETUP\n");

        for (uint8_t i = 0; i < NUM_FLYs; i++) {
            fly[i].visible      = WHITE;
            fly[i].action_image = RANDOM_FLY_ACTION_IMAGE();
            
            // Activate the position generation function so it does not overlap the butterfly
            fly_random_position(i); 
        }

    } break;

    case AR_GAME_FLY_RUN: {
        APP_DBG_SIG("AR_GAME_FLY_RUN\n");
		if(game_state != GAME_STATE_NORMAL)
		{
			return;
		}
        for (uint8_t i = 0; i < NUM_FLYs; i++) {
            if (fly[i].visible == WHITE) {
                fly[i].x -= settingsetup.fly_speed;
                fly[i].action_image++;
                if (fly[i].action_image > AR_GAME_FLY_ACTION_IMAGE_3) {
                    fly[i].action_image = AR_GAME_FLY_ACTION_IMAGE_1;
                }
            }
        }
		
    } break;

    case AR_GAME_FLY_DETONATOR: {
        APP_DBG_SIG("AR_GAME_FLY_DETONATOR\n");
        for (uint8_t i = 0; i < NUM_FLYs; i++) {
            if (fly[i].visible == WHITE) {
                for (uint8_t j = 0; j < MAX_NUM_ARROW; j++) {
                    if (arrow[j].visible == WHITE) {
                       if (IS_COLLIDE(arrow[j].x, arrow[j].y, SIZE_BITMAP_ARROW_X, SIZE_BITMAP_ARROW_Y, \
                                      fly[i].x, fly[i].y, SIZE_BITMAP_FLYs_X, SIZE_BITMAP_FLYs_Y))
                        {
                            // Clear arrow and fly
                            fly[i].visible      = BLACK;
                            arrow[j].visible          = BLACK;

                            // Show bang
                            bang[i].visible           = WHITE;
                            bang[i].action_image      = AR_GAME_BANG_ACTION_IMAGE_1;
                            bang[i].x                 = (fly[i].x > 3 ? fly[i].x - 3 : 0);
                            bang[i].y                 = fly[i].y + 2;

                            // Reset arrow
                            arrow[j].x                = 0;
                            arrow[j].y                = 0;

                            
                            fly_random_position(i);
                            fly[i].action_image = RANDOM_FLY_ACTION_IMAGE();
                            fly[i].visible = WHITE;

                            if (settingsetup.num_arrow < MAX_NUM_ARROW) {
                                settingsetup.num_arrow++;
                            }

                            // Update score and play sound
                            ar_game_score += 10;
							/*==============================
								Kiểm tra Boss
							==============================*/
							if(game_state == GAME_STATE_NORMAL)
							{
								if(ar_game_score >= next_boss_score)
								{
									game_state = GAME_STATE_WARNING;

									warning_timer = 90;      

									next_boss_score += 500;  
								}
							}

                            break;
                        }
                    }
                }
            }
        }
    } break;

    case AR_GAME_FLY_RESET: {
        APP_DBG_SIG("AR_GAME_FLY_RESET\n");
        for (uint8_t i = 0; i < NUM_FLYs; i++) {
            fly[i].visible = BLACK;
        }
    } break;

    default:
        break;
    }
}

void fly_adjust_position(uint8_t index)
{
    uint8_t attempts = 0;

    while(attempts < MAX_POSITION_ATTEMPTS)
    {
        bool overlap = false;

        for(uint8_t i = 0; i < NUM_BUTTERFLIES; i++)
        {
            if(butterfly[i].visible == WHITE)
            {
                if(abs((int)fly[index].x - (int)butterfly[i].x) < SAFE_DISTANCE_X &&
                   abs((int)fly[index].y - (int)butterfly[i].y) < SAFE_DISTANCE_Y)
                {
                    overlap = true;
                    break;
                }
            }
        }

        if(!overlap)
        {
            return;
        }

        fly[index].x = RANDOM_FLY_X();
        attempts++;
    }
}

void fly_random_position(uint8_t index)
{
    bool overlap;
    uint8_t attempts = 0;

    do
    {
        overlap = false;

        fly[index].x = RANDOM_FLY_X();

        if(index == 0)
            fly[index].y = RANDOM_FLY_Y_TOP();
        else if(index == 1)
            fly[index].y = RANDOM_FLY_Y_MIDDLE();
        else
            fly[index].y = RANDOM_FLY_Y_BOTTOM();

        for(uint8_t i = 0; i < NUM_BUTTERFLIES; i++)
        {
            if(butterfly[i].visible == WHITE)
            {
                if(IS_COLLIDE(fly[index].x, fly[index].y, SIZE_BITMAP_FLYs_X + SAFE_DISTANCE_X, SIZE_BITMAP_FLYs_Y + SAFE_DISTANCE_Y,
                              butterfly[i].x, butterfly[i].y, SIZE_BITMAP_BUTTERFLY_X, SIZE_BITMAP_BUTTERFLY_Y))
                {
                    overlap = true;
                    break;
                }
            }
        }
        attempts++;

    } while(overlap && attempts < MAX_POSITION_ATTEMPTS);
}