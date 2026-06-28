#include "ar_game_bee.h"

#include "ar_game_arrow.h"
#include "ar_game_border.h"
#include "ar_game_meteoroid.h"

#define IS_COLLIDE(x1,y1,w1,h1,x2,y2,w2,h2) (((x1) < ((x2) + (w2))) && \
                                             (((x1) + (w1)) > (x2)) && \
                                             ((y1) < ((y2) + (h2))) && \
                                             (((y1) + (h1)) > (y2)))

#define RANDOM_BEE_X() ((rand() % 39) + 130)

#define RANDOM_BEE_Y() (rand() % 40)

#define RANDOM_BEE_ACTION_IMAGE() \
(rand() % AR_GAME_BEE_ACTION_IMAGE_3 + AR_GAME_BEE_ACTION_IMAGE_1)

#define RANDOM_BEE_Y_TOP()      (rand() % 4)
#define RANDOM_BEE_Y_MIDDLE()   (18 + rand() % 4)
#define RANDOM_BEE_Y_BOTTOM()   (36 + rand() % 4)

#define SAFE_DISTANCE_X    26
#define SAFE_DISTANCE_Y    12
#define MAX_POSITION_ATTEMPTS 20

#define BEE_MIN_X            0
#define BEE_MAX_X            108
#define BEE_MIN_Y            0
#define BEE_MAX_Y            44

static const int8_t bee_sin_table[32] =
{
     0, 1, 2, 3, 4, 5, 5, 6,
     6, 5, 5, 4, 3, 2, 1, 0,
     0,-1,-2,-3,-4,-5,-5,-6,
    -6,-5,-5,-4,-3,-2,-1, 0
};


/* ========================================================================= */
/* CẤU HÌNH THỜI GIAN XUẤT HIỆN TRONG VÒNG 5 GIÂY                             */
/* ========================================================================= */
// Giả định chu kỳ gửi signal RUN của game bạn là 20ms (tương đương 50 lần/giây). 
// Nếu game chạy quá nhanh hoặc quá chậm, bạn chỉ cần điều chỉnh số 50 này.
#define BEE_TICK_PER_SECOND   30 
#define BEE_MAX_SPAWN_5_SEC   (5 * BEE_TICK_PER_SECOND) // Tổng số tick trong 5 giây (250 ticks)

ar_game_bee_t bee[NUM_BEE];

static uint8_t bee_random_y()
{
    switch(rand() % 3)
    {
        case 0:
            return RANDOM_BEE_Y_TOP();

        case 1:
            return RANDOM_BEE_Y_MIDDLE();

        default:
            return RANDOM_BEE_Y_BOTTOM();
    }
}

static void bee_random_position(uint8_t index)
{
    bool overlap;
    uint8_t attempts = 0;

    do
    {
        overlap = false;

        bee[index].x = RANDOM_BEE_X();
        bee[index].y = bee_random_y();

        bee[index].base_y = bee[index].y;

        bee[index].wave_index = rand() % 32;

        bee[index].wave_speed = 2 + rand() % 4;

        bee[index].wave_counter = 0;

        for(uint8_t i = 0; i < NUM_METEOROIDS; i++)
        {
            if(meteoroid[i].visible == WHITE)
            {
                if(IS_COLLIDE(bee[index].x, bee[index].y, SIZE_BITMAP_BEE_X + SAFE_DISTANCE_X, SIZE_BITMAP_BEE_Y + SAFE_DISTANCE_Y,
                              meteoroid[i].x, meteoroid[i].y, SIZE_BITMAP_METEOROIDS_X, SIZE_BITMAP_METEOROIDS_Y))
                {
                    overlap = true;
                    break;
                }
            }
        }
        attempts++;

    } while(overlap && attempts < MAX_POSITION_ATTEMPTS);
}

void ar_game_bee_handle(ak_msg_t *msg) {
    switch (msg->sig) {

    case AR_GAME_BEE_SETUP: {

        APP_DBG_SIG("AR_GAME_BEE_SETUP\n");

        for(uint8_t i = 0; i < NUM_BEE; i++) {
            bee[i].x = RANDOM_BEE_X();
            bee[i].y = RANDOM_BEE_Y();
            bee[i].visible = BLACK;
            bee[i].show_minus20 = false;
            bee[i].minus20_timer = 0;
            bee[i].action_image = RANDOM_BEE_ACTION_IMAGE();
            
            // Lần đầu vào game, ong xuất hiện ngẫu nhiên từ giây thứ 1 đến giây thứ 5
            bee[i].spawn_delay = (rand() % (4 * BEE_TICK_PER_SECOND)) + BEE_TICK_PER_SECOND; 
        }

    } break;

    case AR_GAME_BEE_RUN:
    {
        APP_DBG_SIG("AR_GAME_BEE_RUN\n");

        if(game_state != GAME_STATE_NORMAL)
        {
            return;
        }

        for(uint8_t i = 0; i < NUM_BEE; i++)
        {
            if(bee[i].show_minus20)
            {
                if(bee[i].minus20_timer)
                {
                    bee[i].minus20_timer--;

                    if((bee[i].minus20_timer % 2)==0)
                    {
                        bee[i].minus20_y--;
                    }
                }
                else
                {
                    bee[i].show_minus20 = false;
                }
            }
            /*==========================
                Bee đang ẩn -> Chờ xuất hiện
            ==========================*/
            if(bee[i].visible == BLACK)
            {
                if(bee[i].spawn_delay > 0)
                {
                    bee[i].spawn_delay--;
                }
                else
                {
                    bee_random_position(i);

                    bee[i].visible = WHITE;
                    bee[i].action_image = RANDOM_BEE_ACTION_IMAGE();
                }
                continue;
            }

            /*==========================
                Bee đang bay
            ==========================*/
            bee[i].x -= settingsetup.meteoroid_speed;

            bee[i].wave_counter++;

            if(bee[i].wave_counter >= bee[i].wave_speed)
            {
                bee[i].wave_counter = 0;

                bee[i].wave_index++;

                if(bee[i].wave_index >= 32)
                {
                    bee[i].wave_index = 0;
                }
            }

            bee[i].y =
            bee[i].base_y +
            bee_sin_table[bee[i].wave_index];
            
            if(bee[i].y < 0)
            {
                bee[i].y = 0;
            }

            if(bee[i].y > 54)
            {
                bee[i].y = 54;
            }

            // Nếu ong bay thoát khỏi biên trái (người chơi né được)
            if(bee[i].x <= AXIS_X_BORDER)
            {
                bee[i].visible = BLACK;
                
                // Tạo thời gian chờ ngẫu nhiên hoàn toàn bên trong giới hạn 5 giây 
                // Có thể là 0.5s, 1s, 3s, tối đa là 5s.
                bee[i].spawn_delay = rand() % BEE_MAX_SPAWN_5_SEC;
                continue;
            }

            bee[i].action_image++;

            if(bee[i].action_image > AR_GAME_BEE_ACTION_IMAGE_3)
            {
                bee[i].action_image = AR_GAME_BEE_ACTION_IMAGE_1;
            }
        }

    } break;

    case AR_GAME_BEE_DETONATOR: {

        APP_DBG_SIG("AR_GAME_BEE_DETONATOR\n");

        for(uint8_t i = 0; i < NUM_BEE; i++) {

            if(bee[i].visible == WHITE) {

                for(uint8_t j = 0; j < MAX_NUM_ARROW; j++)
                {
                    if(arrow[j].visible == WHITE) {

                        if(IS_COLLIDE(arrow[j].x, arrow[j].y, SIZE_BITMAP_ARROW_X, SIZE_BITMAP_ARROW_Y,
                                      bee[i].x, bee[i].y, SIZE_BITMAP_BEE_X, SIZE_BITMAP_BEE_Y))
                        {
                            arrow[j].visible = BLACK;
                            arrow[j].x = 0;
                            arrow[j].y = 0;

                            bee[i].show_minus20 = true;

                            bee[i].minus20_timer = 20;

                            bee[i].minus20_x = bee[i].x;

                            bee[i].minus20_y = bee[i].y;
                            // Khi ong bị bắn trúng, ẩn đi và reset thời gian xuất hiện tiếp theo ngẫu nhiên trong 5s
                            bee[i].visible = BLACK;
                            bee[i].spawn_delay = rand() % BEE_MAX_SPAWN_5_SEC;
                            
                            bee[i].action_image = RANDOM_BEE_ACTION_IMAGE();

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

    case AR_GAME_BEE_RESET: {

        APP_DBG_SIG("AR_GAME_BEE_RESET\n");

        for(uint8_t i = 0; i < NUM_BEE; i++) {
            bee[i].visible = BLACK;
            bee[i].spawn_delay = 0;
        }

    } break;

    default:
        break;
    }
}