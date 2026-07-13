#include "ar_game_butterfly.h"

#include "ar_game_arrow.h"
#include "ar_game_border.h"
#include "ar_game_fly.h"

#define IS_COLLIDE(x1,y1,w1,h1,x2,y2,w2,h2) (((x1) < ((x2) + (w2))) && \
                                             (((x1) + (w1)) > (x2)) && \
                                             ((y1) < ((y2) + (h2))) && \
                                             (((y1) + (h1)) > (y2)))

#define RANDOM_BUTTERFLY_X() ((rand() % 39) + 130)

#define RANDOM_BUTTERFLY_Y() (rand() % 40)

// #define RANDOM_BUTTERFLY_ACTION_IMAGE() \
// (rand() % AR_GAME_BUTTERFLY_ACTION_IMAGE_3 + AR_GAME_BUTTERFLY_ACTION_IMAGE_1)

#define RANDOM_BUTTERFLY_Y_TOP()      (rand() % 4)
#define RANDOM_BUTTERFLY_Y_MIDDLE()   (16 + rand() % 4)
#define RANDOM_BUTTERFLY_Y_BOTTOM()   (32 + rand() % 4)

#define SAFE_DISTANCE_X    26
#define SAFE_DISTANCE_Y    12
#define MAX_POSITION_ATTEMPTS 20

#define BUTTERFLY_MIN_X            0
#define BUTTERFLY_MAX_X            108
#define BUTTERFLY_MIN_Y            0
#define BUTTERFLY_MAX_Y            44

static const int8_t butterfly_sin_table[32] =
{
     0, 1, 2, 3, 4, 5, 5, 6,
     6, 5, 5, 4, 3, 2, 1, 0,
     0,-1,-2,-3,-4,-5,-5,-6,
    -6,-5,-5,-4,-3,-2,-1, 0
};


/* ========================================================================= */
/* CONFIGURE APPEARANCE TIMING WITHIN 5 SECONDS                             */
/* ========================================================================= */

#define BUTTERFLY_TICK_PER_SECOND   30 
#define BUTTERFLY_MAX_SPAWN_5_SEC   (5 * BUTTERFLY_TICK_PER_SECOND) 

ar_game_butterfly_t butterfly[NUM_BUTTERFLIES];

static uint8_t butterfly_random_y()
{
    switch(rand() % 3)
    {
        case 0:
            return RANDOM_BUTTERFLY_Y_TOP();

        case 1:
            return RANDOM_BUTTERFLY_Y_MIDDLE();

        default:
            return RANDOM_BUTTERFLY_Y_BOTTOM();
    }
}

static void butterfly_random_position(uint8_t index)
{
    bool overlap;
    uint8_t attempts = 0;

    do
    {
        overlap = false;

        butterfly[index].x = RANDOM_BUTTERFLY_X();
        butterfly[index].y = butterfly_random_y();

        butterfly[index].base_y = butterfly[index].y;

        butterfly[index].wave_index = rand() % 32;

        butterfly[index].wave_speed = 2 + rand() % 4;

        butterfly[index].wave_counter = 0;

        for(uint8_t i = 0; i < NUM_FLYs; i++)
        {
            if(fly[i].visible == WHITE)
            {
                if(IS_COLLIDE(butterfly[index].x, butterfly[index].y, SIZE_BITMAP_BUTTERFLY_X + SAFE_DISTANCE_X, SIZE_BITMAP_BUTTERFLY_Y + SAFE_DISTANCE_Y,
                              fly[i].x, fly[i].y, SIZE_BITMAP_FLYs_X, SIZE_BITMAP_FLYs_Y))
                {
                    overlap = true;
                    break;
                }
            }
        }
        attempts++;

    } while(overlap && attempts < MAX_POSITION_ATTEMPTS);
}

void ar_game_butterfly_handle(ak_msg_t *msg) {
    switch (msg->sig) {

    case AR_GAME_BUTTERFLY_SETUP: {

        APP_DBG_SIG("AR_GAME_BUTTERFLY_SETUP\n");

        for(uint8_t i = 0; i < NUM_BUTTERFLIES; i++) {
            butterfly[i].x = RANDOM_BUTTERFLY_X();
            butterfly[i].y = RANDOM_BUTTERFLY_Y();
            butterfly[i].visible = BLACK;
            butterfly[i].show_minus20 = false;
            butterfly[i].minus20_timer = 0;
            //butterfly[i].action_image = RANDOM_BUTTERFLY_ACTION_IMAGE();
            butterfly[i].action_image = AR_GAME_BUTTERFLY_ACTION_IMAGE_1;
            // On first game start, butterflies appear randomly between 1 and 5 seconds
            butterfly[i].spawn_delay = (rand() % (4 * BUTTERFLY_TICK_PER_SECOND)) + BUTTERFLY_TICK_PER_SECOND; 
        }

    } break;

    case AR_GAME_BUTTERFLY_RUN:
    {
        APP_DBG_SIG("AR_GAME_BUTTERFLY_RUN\n");

        if(game_state != GAME_STATE_NORMAL)
        {
            return;
        }

        for(uint8_t i = 0; i < NUM_BUTTERFLIES; i++)
        {
            if(butterfly[i].show_minus20)
            {
                if(butterfly[i].minus20_timer)
                {
                    butterfly[i].minus20_timer--;

                    if((butterfly[i].minus20_timer % 2)==0)
                    {
                        butterfly[i].minus20_y--;
                    }
                }
                else
                {
                    butterfly[i].show_minus20 = false;
                }
            }
            /*==========================
                Butterfly is hidden -> waiting to appear
            ==========================*/
            if(butterfly[i].visible == BLACK)
            {
                if(butterfly[i].spawn_delay > 0)
                {
                    butterfly[i].spawn_delay--;
                }
                else
                {
                    butterfly_random_position(i);

                    butterfly[i].visible = WHITE;
                    //butterfly[i].action_image = RANDOM_BUTTERFLY_ACTION_IMAGE();
                    butterfly[i].action_image = AR_GAME_BUTTERFLY_ACTION_IMAGE_1;
                }
                continue;
            }

            /*==========================
                Butterfly is flying
            ==========================*/
            butterfly[i].x -= settingsetup.fly_speed;

            butterfly[i].wave_counter++;

            if(butterfly[i].wave_counter >= butterfly[i].wave_speed)
            {
                butterfly[i].wave_counter = 0;

                butterfly[i].wave_index++;

                if(butterfly[i].wave_index >= 32)
                {
                    butterfly[i].wave_index = 0;
                }
            }

            butterfly[i].y =
            butterfly[i].base_y +
            butterfly_sin_table[butterfly[i].wave_index];
            
            if(butterfly[i].y < BUTTERFLY_MIN_Y)
            {
                butterfly[i].y = BUTTERFLY_MIN_Y;
            }

            if(butterfly[i].y > BUTTERFLY_MAX_Y)
            {
                butterfly[i].y = BUTTERFLY_MAX_Y;
            }

            // If the butterfly flies off the left edge (player dodged it)
            if(butterfly[i].x <= AXIS_X_BORDER)
            {
                butterfly[i].visible = BLACK;
                
                // Set a random delay fully within the 5-second limit
                butterfly[i].spawn_delay = rand() % BUTTERFLY_MAX_SPAWN_5_SEC;
                continue;
            }

            // butterfly[i].action_image++;

            // if(butterfly[i].action_image > AR_GAME_BUTTERFLY_ACTION_IMAGE_3)
            // {
            //     butterfly[i].action_image = AR_GAME_BUTTERFLY_ACTION_IMAGE_1;
            // }
            butterfly[i].action_image = AR_GAME_BUTTERFLY_ACTION_IMAGE_1;
        }

    } break;

    case AR_GAME_BUTTERFLY_DETONATOR: {

        APP_DBG_SIG("AR_GAME_BUTTERFLY_DETONATOR\n");

        for(uint8_t i = 0; i < NUM_BUTTERFLIES; i++) {

            if(butterfly[i].visible == WHITE) {

                for(uint8_t j = 0; j < MAX_NUM_ARROW; j++)
                {
                    if(arrow[j].visible == WHITE) {

                        if(IS_COLLIDE(arrow[j].x, arrow[j].y, SIZE_BITMAP_ARROW_X, SIZE_BITMAP_ARROW_Y,
                                      butterfly[i].x, butterfly[i].y, SIZE_BITMAP_BUTTERFLY_X, SIZE_BITMAP_BUTTERFLY_Y))
                        {
                            arrow[j].visible = BLACK;
                            arrow[j].x = 0;
                            arrow[j].y = 0;

                            butterfly[i].show_minus20 = true;

                            butterfly[i].minus20_timer = 20;

                            butterfly[i].minus20_x = butterfly[i].x;

                            butterfly[i].minus20_y = butterfly[i].y;
                            
                            butterfly[i].visible = BLACK;
                            butterfly[i].spawn_delay = rand() % BUTTERFLY_MAX_SPAWN_5_SEC;
                            
                            //butterfly[i].action_image = RANDOM_BUTTERFLY_ACTION_IMAGE();
                            butterfly[i].action_image = AR_GAME_BUTTERFLY_ACTION_IMAGE_1;

                            if(ar_game_score >= 20) {
                                ar_game_score -= 20;
                            }
                            else {
                                ar_game_score = 0;
                            }

                            BUZZER_PlaySound(BUZZER_SOUND_BEE);
                            break;
                        }
                    }
                }
            }
        }

    } break;

    case AR_GAME_BUTTERFLY_RESET: {

        APP_DBG_SIG("AR_GAME_BUTTERFLY_RESET\n");

        for(uint8_t i = 0; i < NUM_BUTTERFLIES; i++) {
            butterfly[i].visible = BLACK;
            butterfly[i].spawn_delay = 0;
        }

    } break;

    default:
        break;
    }
}