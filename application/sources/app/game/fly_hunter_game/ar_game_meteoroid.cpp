#include "ar_game_meteoroid.h"

#include "ar_game_arrow.h"
#include "ar_game_bang.h"
#include "ar_game_border.h"
#include "scr_fly_hunter_game.h"
#include "ar_game_bee.h"

#define IS_COLLIDE(x1,y1,w1,h1,x2,y2,w2,h2) (((x1) < ((x2) + (w2))) && \
                                             (((x1) + (w1)) > (x2)) && \
                                             ((y1) < ((y2) + (h2))) && \
                                             (((y1) + (h1)) > (y2)))

#define RANDOM_METEOROID_ACTION_IMAGE() (rand() % AR_GAME_METEOROID_ACTION_IMAGE_3 + AR_GAME_METEOROID_ACTION_IMAGE_1)

#define SAFE_DISTANCE_X     35
#define SAFE_DISTANCE_Y     12
#define MAX_POSITION_ATTEMPTS 20

ar_game_meteoroid_t meteoroid[NUM_METEOROIDS];

void ar_game_meteoroid_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case AR_GAME_METEOROID_SETUP: {
        APP_DBG_SIG("AR_GAME_METEOROID_SETUP\n");

        for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
            meteoroid[i].visible      = WHITE;
            meteoroid[i].action_image = RANDOM_METEOROID_ACTION_IMAGE();
            
            // Kích hoạt hàm tính toán sinh vị trí ngẫu nhiên không đè lên Ong
            meteoroid_random_position(i); 
        }

    } break;

    case AR_GAME_METEOROID_RUN: {
        APP_DBG_SIG("AR_GAME_METEOROID_RUN\n");
		if(game_state != GAME_STATE_NORMAL)
		{
			return;
		}
        for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
            if (meteoroid[i].visible == WHITE) {
                meteoroid[i].x -= settingsetup.meteoroid_speed;
                meteoroid[i].action_image++;
                if (meteoroid[i].action_image > AR_GAME_METEOROID_ACTION_IMAGE_3) {
                    meteoroid[i].action_image = AR_GAME_METEOROID_ACTION_IMAGE_1;
                }
            }
        }
		
    } break;

    case AR_GAME_METEOROID_DETONATOR: {
        APP_DBG_SIG("AR_GAME_METEOROID_DETONATOR\n");
        for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
            if (meteoroid[i].visible == WHITE) {
                for (uint8_t j = 0; j < MAX_NUM_ARROW; j++) {
                    if (arrow[j].visible == WHITE) {
                       if (IS_COLLIDE(arrow[j].x, arrow[j].y, SIZE_BITMAP_ARROW_X, SIZE_BITMAP_ARROW_Y, \
                                      meteoroid[i].x, meteoroid[i].y, SIZE_BITMAP_METEOROIDS_X, SIZE_BITMAP_METEOROIDS_Y))
                        {
                            // Clear arrow and meteoroid
                            meteoroid[i].visible      = BLACK;
                            arrow[j].visible          = BLACK;

                            // Show bang
                            bang[i].visible           = WHITE;
                            bang[i].action_image      = AR_GAME_BANG_ACTION_IMAGE_1;
                            bang[i].x                 = (meteoroid[i].x > 5 ? meteoroid[i].x - 5 : 0);
                            bang[i].y                 = meteoroid[i].y + 2;

                            // Reset arrow
                            arrow[j].x                = 0;
                            arrow[j].y                = 0;

                            // Tái sinh thiên thạch mới bằng hàm random vị trí có kiểm tra va chạm chéo với Ong
                            meteoroid_random_position(i);
                            meteoroid[i].action_image = RANDOM_METEOROID_ACTION_IMAGE();
                            meteoroid[i].visible = WHITE;

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

									warning_timer = 90;      // khoảng 3 giây

									next_boss_score += 500;  // Boss tiếp theo
								}
							}

                            break;
                        }
                    }
                }
            }
        }
    } break;

    case AR_GAME_METEOROID_RESET: {
        APP_DBG_SIG("AR_GAME_METEOROID_RESET\n");
        for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
            meteoroid[i].visible = BLACK;
        }
    } break;

    default:
        break;
    }
}

void meteoroid_adjust_position(uint8_t index)
{
    uint8_t attempts = 0;

    while(attempts < MAX_POSITION_ATTEMPTS)
    {
        bool overlap = false;

        for(uint8_t i = 0; i < NUM_BEE; i++)
        {
            if(bee[i].visible == WHITE)
            {
                if(abs((int)meteoroid[index].x - (int)bee[i].x) < SAFE_DISTANCE_X &&
                   abs((int)meteoroid[index].y - (int)bee[i].y) < SAFE_DISTANCE_Y)
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

        meteoroid[index].x = RANDOM_METEOROID_X();
        attempts++;
    }
}

void meteoroid_random_position(uint8_t index)
{
    bool overlap;
    uint8_t attempts = 0;

    do
    {
        overlap = false;

        meteoroid[index].x = RANDOM_METEOROID_X();

        if(index == 0)
            meteoroid[index].y = RANDOM_METEOROID_Y_TOP();
        else if(index == 1)
            meteoroid[index].y = RANDOM_METEOROID_Y_MIDDLE();
        else
            meteoroid[index].y = RANDOM_METEOROID_Y_BOTTOM();

        for(uint8_t i = 0; i < NUM_BEE; i++)
        {
            if(bee[i].visible == WHITE)
            {
                if(IS_COLLIDE(meteoroid[index].x, meteoroid[index].y, SIZE_BITMAP_METEOROIDS_X + SAFE_DISTANCE_X, SIZE_BITMAP_METEOROIDS_Y + SAFE_DISTANCE_Y,
                              bee[i].x, bee[i].y, SIZE_BITMAP_BEE_X, SIZE_BITMAP_BEE_Y))
                {
                    overlap = true;
                    break;
                }
            }
        }
        attempts++;

    } while(overlap && attempts < MAX_POSITION_ATTEMPTS);
}