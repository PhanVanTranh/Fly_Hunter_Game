#include "scr_fly_hunter_game.h"

#include "screens.h"

/*****************************************************************************/
/* Variable Declaration - fly_hunter game screen */
/*****************************************************************************/
extern uint8_t border_flash;
uint8_t ar_game_state;
ar_game_setting_t settingsetup;

game_state_t game_state = GAME_STATE_NORMAL;

uint16_t warning_timer = 0;
uint16_t next_boss_score = 500;

/*****************************************************************************/
/* View - fly_hunter game screen*/
/*****************************************************************************/
void ar_game_frame_display() {
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	for(uint8_t i = 0; i < player_life; i++) {
    view_render.drawBitmap(
        2 + i * 10,
        55,
        bitmap_heart,
        8,
        8,
        WHITE
    );
	}
	view_render.setCursor(60,54);
	view_render.print(" Score:");
	view_render.print(ar_game_score);
	view_render.drawLine(0, LCD_HEIGHT, 	LCD_WIDTH, LCD_HEIGHT,		WHITE);
	view_render.drawLine(0, LCD_HEIGHT-12, 	LCD_WIDTH, LCD_HEIGHT-12,	WHITE);
	view_render.drawRect(0, 0, 128, 64, 1);
}

void ar_game_fly_hunter_display() {
	if (fly_hunter.visible == WHITE) {
		if (fly_hunter.action_image == AR_GAME_FLY_HUNTER_ACTION_IMAGE_1) {
			view_render.drawBitmap(	fly_hunter.x, \
					fly_hunter.y - 10, \
					bitmap_fly_hunter_I, \
					SIZE_BITMAP_FLY_HUNTER_X, \
					SIZE_BITMAP_FLY_HUNTER_Y, \
					WHITE);
		}
		else if (fly_hunter.action_image == AR_GAME_FLY_HUNTER_ACTION_IMAGE_2) {
			view_render.drawBitmap(	fly_hunter.x, \
					fly_hunter.y - 10, \
					bitmap_fly_hunter_II, \
					SIZE_BITMAP_FLY_HUNTER_X, \
					SIZE_BITMAP_FLY_HUNTER_Y, \
					WHITE);
		}
	}
}

void ar_game_arrow_display() {
	for (uint8_t i = 0; i < MAX_NUM_ARROW; i++) {
		if (arrow[i].visible == WHITE) {
			view_render.drawBitmap(	arrow[i].x, \
									arrow[i].y, \
									bitmap_arrow, \
									SIZE_BITMAP_ARROW_X, \
									SIZE_BITMAP_ARROW_Y, \
									WHITE);
		}
	}
}

void ar_game_fly_display() {
	for (uint8_t i = 0; i < NUM_FLYs; i++) {
		if (fly[i].visible == WHITE) {
			// if (fly[i].action_image == AR_GAME_FLY_ACTION_IMAGE_1) {
			// 	view_render.drawBitmap(	fly[i].x, \
			// 							fly[i].y, \
			// 							bitmap_fly_I, \
			// 							SIZE_BITMAP_FLYs_X, \
			// 							SIZE_BITMAP_FLYs_Y, \
			// 	 					WHITE);
			// }
			// else if (fly[i].action_image == AR_GAME_FLY_ACTION_IMAGE_2) {
			// 	view_render.drawBitmap(	fly[i].x, \
			// 							fly[i].y, \
			// 							bitmap_fly_II, \
			// 							SIZE_BITMAP_FLYs_X, \
			// 							SIZE_BITMAP_FLYs_Y, \
			// 							WHITE);
			// }
			// else if (fly[i].action_image == AR_GAME_FLY_ACTION_IMAGE_3) {
			// 	view_render.drawBitmap(	fly[i].x, \
			// 							fly[i].y, \
			// 							bitmap_fly_III, \
			// 							SIZE_BITMAP_FLYs_X, \
			// 							SIZE_BITMAP_FLYs_Y, \
			// 	 						WHITE);
			// }

			//fly[i].action_image == AR_GAME_FLY_ACTION_IMAGE_1) 
				view_render.drawBitmap(	fly[i].x, \
										fly[i].y, \
										bitmap_fly_I, \
										SIZE_BITMAP_FLYs_X, \
										SIZE_BITMAP_FLYs_Y, \
				 					WHITE);
		}
	}
}

void ar_game_butterfly_display() {

    for(uint8_t i = 0; i < NUM_BUTTERFLIES; i++) {

        if(butterfly[i].visible == WHITE) {

            // switch(butterfly[i].action_image) {

            // case AR_GAME_BUTTERFLY_ACTION_IMAGE_1:

            //     view_render.drawBitmap(
            //         butterfly[i].x,
            //         butterfly[i].y,
            //         bitmap_butterfly_I,
            //         SIZE_BITMAP_BUTTERFLY_X,
            //         SIZE_BITMAP_BUTTERFLY_Y,
            //         WHITE);

            //     break;

            // case AR_GAME_BUTTERFLY_ACTION_IMAGE_2:

            //     view_render.drawBitmap(
            //         butterfly[i].x,
            //         butterfly[i].y,
            //         bitmap_butterfly_II,
            //         SIZE_BITMAP_BUTTERFLY_X,
            //         SIZE_BITMAP_BUTTERFLY_Y,
            //         WHITE);

            //     break;

            // case AR_GAME_BUTTERFLY_ACTION_IMAGE_3:

            //     view_render.drawBitmap(
            //         butterfly[i].x,
            //         butterfly[i].y,
            //         bitmap_butterfly_III,
            //         SIZE_BITMAP_BUTTERFLY_X,
            //         SIZE_BITMAP_BUTTERFLY_Y,
            //         WHITE);

            //     break;
            // }

			view_render.drawBitmap(
                    butterfly[i].x,
                    butterfly[i].y,
                    bitmap_butterfly_I,
                    SIZE_BITMAP_BUTTERFLY_X,
                    SIZE_BITMAP_BUTTERFLY_Y,
                    WHITE);
        }
    }
	for(uint8_t i=0;i<NUM_BUTTERFLIES;i++)
	{
		if(butterfly[i].show_minus20 &&
			(butterfly[i].minus20_timer % 2 == 0))
			{
				view_render.setCursor(
					butterfly[i].minus20_x,
					butterfly[i].minus20_y);

				view_render.print("-20");
			}
	}
}


void ar_game_bang_display() {
	for (uint8_t i = 0; i < NUM_BANG; i++) {
		if (bang[i].visible == WHITE) {
			if (bang[i].action_image == AR_GAME_BANG_ACTION_IMAGE_1) {
				view_render.drawBitmap(	bang[i].x, \
										bang[i].y, \
										bitmap_bang_I, \
										SIZE_BITMAP_BANG_I_X, \
										SIZE_BITMAP_BANG_I_Y, \
										WHITE);
			}
			else if (bang[i].action_image == AR_GAME_BANG_ACTION_IMAGE_2) {
				view_render.drawBitmap(	bang[i].x, \
										bang[i].y, \
										bitmap_bang_II, \
										SIZE_BITMAP_BANG_I_X, \
										SIZE_BITMAP_BANG_I_Y, \
				 						WHITE);
			}
			else if (bang[i].action_image == AR_GAME_BANG_ACTION_IMAGE_3) {
				view_render.drawBitmap( bang[i].x + 2, \
										bang[i].y - 1, \
										bitmap_bang_III, \
										SIZE_BITMAP_BANG_II_X, \
										SIZE_BITMAP_BANG_II_Y, \
				 						WHITE);
			}
		}
	}
}

void ar_game_border_display() {
	if (border.visible == WHITE) {
		if (border_flash > 0) {

			if (border_flash % 2) {

				view_render.drawFastVLine(
					border.x,
					AXIS_Y_BORDER_ON,
					AXIS_Y_BORDER_UNDER,
					WHITE);

				for (uint8_t i = 0; i < NUM_FLYs; i++) {
					view_render.fillCircle(
						border.x,
						fly[i].y + 5,
						1,
						WHITE);
				}
			}

			border_flash--;
		}
		else {

			view_render.drawFastVLine(
				border.x,
				AXIS_Y_BORDER_ON,
				AXIS_Y_BORDER_UNDER,
				WHITE);

			for (uint8_t i = 0; i < NUM_FLYs; i++) {
				view_render.fillCircle(
					border.x,
					fly[i].y + 3,
					1,
					WHITE);
			}
		}
	}
}

void ar_game_boss_display()
{
    if(!boss.visible)
        return;

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

    if(boss.hit_flash)
    {
        if(boss.hit_flash & 1)
        {
            return;
        }
    }

    /**************************************************
        Draw Boss
    **************************************************/

    view_render.drawBitmap(

        boss.x + boss.shake_offset,

        boss.y,

        bitmap_fly_boss,

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

}

void ar_game_boss_bullet_display()
{
    for(uint8_t i = 0; i < NUM_BOSS_BULLET; i++)
    {
        if(boss_bullet[i].visible == BLACK)
            continue;

        view_render.drawBitmap(
            boss_bullet[i].x,
            boss_bullet[i].y,
            bitmap_boss_bullet,
            SIZE_BITMAP_BOSS_BULLET_X,
            SIZE_BITMAP_BOSS_BULLET_Y,
            WHITE);
    }
}

static void view_scr_fly_hunter_game();

view_dynamic_t dyn_view_item_fly_hunter_game = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_fly_hunter_game
};

view_screen_t scr_fly_hunter_game = {
	&dyn_view_item_fly_hunter_game,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_fly_hunter_game() {
	if (ar_game_state == GAME_PLAY) {
		ar_game_frame_display();
		ar_game_fly_hunter_display();
		ar_game_arrow_display();
		ar_game_fly_display();
		ar_game_butterfly_display();
		ar_game_bang_display();
		ar_game_boss_display();
		ar_game_border_display();
		ar_game_boss_bullet_display();

		if(game_state == GAME_STATE_WARNING)
		{
			if((warning_timer / 5) % 2)
			{
				BUZZER_PlaySound(BUZZER_SOUND_3BEEP);
				
				view_render.setTextColor(WHITE);

				view_render.setTextSize(2);
				view_render.setCursor(30,15);
				view_render.print("WARNING");

				view_render.setTextSize(1);
				view_render.setCursor(30,38);
				view_render.print("BOSS INCOMING");
			}
		}

		if(border_flash) {
			view_render.drawRect(0,0,128,64,WHITE);
			view_render.drawRect(1,1,126,62,WHITE);
			view_render.drawRect(2,2,124,60,WHITE);
			view_render.drawRect(3,3,122,58,WHITE);

			border_flash--;
		}
	}
}

/*****************************************************************************/
/* Handle - fly_hunter game screen */
/*****************************************************************************/
void rank_ranking() {
	if (gamescore.score_now > gamescore.score_1st) {
		gamescore.score_3rd = gamescore.score_2nd;
		gamescore.score_2nd = gamescore.score_1st;
		gamescore.score_1st = gamescore.score_now;
	}
	else if (gamescore.score_now > gamescore.score_2nd) {
		gamescore.score_3rd = gamescore.score_2nd;
		gamescore.score_2nd = gamescore.score_now;
	}
	else if (gamescore.score_now > gamescore.score_3rd) {
		gamescore.score_3rd = gamescore.score_now;
	}
}

void scr_fly_hunter_game_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		// Get setting data
		ar_game_setting_read(&settingsetup);

		// Setup game Object
		task_post_pure_msg(AR_GAME_FLY_HUNTER_ID, 	 	AR_GAME_FLY_HUNTER_SETUP);
		task_post_pure_msg(AR_GAME_ARROW_ID, 	 	AR_GAME_ARROW_SETUP);
		task_post_pure_msg(AR_GAME_FLY_ID, 	AR_GAME_FLY_SETUP);
		task_post_pure_msg(AR_GAME_BANG_ID, 	 	AR_GAME_BANG_SETUP);
		task_post_pure_msg(AR_GAME_BORDER_ID, 	 	AR_GAME_BORDER_SETUP);
		task_post_pure_msg(AR_GAME_BUTTERFLY_ID,           AR_GAME_BUTTERFLY_SETUP);
		task_post_pure_msg(AR_GAME_BOSS_ID,         AR_GAME_BOSS_SETUP);
		task_post_pure_msg(AR_GAME_BOSS_BULLET_ID,  AR_GAME_BOSS_BULLET_SETUP);
		// Set state 'GAME_PLAY' & remove idle screen timer
		ar_game_state = GAME_PLAY;
		game_state = GAME_STATE_NORMAL;

		warning_timer = 0;

		next_boss_score = 500;

		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);

		// Setup timer
		timer_set(	AC_TASK_DISPLAY_ID, \
					AR_GAME_TIME_TICK, \
					AR_GAME_TIME_TICK_INTERVAL, \
					TIMER_PERIODIC);
	} break;

	case AR_GAME_TIME_TICK: {
		APP_DBG_SIG("AR_GAME_TIME_TICK\n");
		// Time tick
		task_post_pure_msg(AR_GAME_FLY_HUNTER_ID, 		AR_GAME_FLY_HUNTER_UPDATE);
		task_post_pure_msg(AR_GAME_ARROW_ID, 		AR_GAME_ARROW_RUN);
		task_post_pure_msg(AR_GAME_FLY_ID, 	AR_GAME_FLY_RUN);
		task_post_pure_msg(AR_GAME_FLY_ID, 	AR_GAME_FLY_DETONATOR);
		task_post_pure_msg(AR_GAME_BANG_ID, 		AR_GAME_BANG_UPDATE);
		task_post_pure_msg(AR_GAME_BORDER_ID, 		AR_GAME_LEVEL_UP);
		task_post_pure_msg(AR_GAME_BORDER_ID, 		AR_GAME_CHECK_GAME_OVER);
		task_post_pure_msg(AR_GAME_BUTTERFLY_ID,           AR_GAME_BUTTERFLY_RUN);
		task_post_pure_msg(AR_GAME_BUTTERFLY_ID,           AR_GAME_BUTTERFLY_DETONATOR);
		task_post_pure_msg(AR_GAME_BOSS_ID,         AR_GAME_BOSS_RUN);
		task_post_pure_msg(AR_GAME_BOSS_ID,         AR_GAME_BOSS_DETONATOR);
		task_post_pure_msg(AR_GAME_BOSS_BULLET_ID,  AR_GAME_BOSS_BULLET_RUN);
		

		if(game_state == GAME_STATE_NORMAL &&
		ar_game_score >= next_boss_score)
		{
			game_state = GAME_STATE_WARNING;
			warning_timer = 40;
			BUZZER_PlaySound(BUZZER_SOUND_WARNING);
		}

		if(game_state == GAME_STATE_WARNING)
		{
			//BUZZER_PlaySound(BUZZER_SOUND_WARNING);
			if(warning_timer > 0)
			{
				warning_timer--;
			}
			else
			{
				game_state = GAME_STATE_BOSS;
				task_post_pure_msg(
       			 AR_GAME_BOSS_ID,
        		AR_GAME_BOSS_SPAWN);
			}
		}

	} break;

	case AR_GAME_RESET: {
		APP_DBG_SIG("AR_GAME_RESET\n");
   
		// Stop timer tick
		timer_remove_attr(AC_TASK_DISPLAY_ID, AR_GAME_TIME_TICK);

		// Reset game Object
		task_post_pure_msg(AR_GAME_FLY_HUNTER_ID, 		AR_GAME_FLY_HUNTER_RESET);
		task_post_pure_msg(AR_GAME_ARROW_ID, 		AR_GAME_ARROW_RESET);
		task_post_pure_msg(AR_GAME_FLY_ID,	AR_GAME_FLY_RESET);
		task_post_pure_msg(AR_GAME_BANG_ID, 		AR_GAME_BANG_RESET);
		task_post_pure_msg(AR_GAME_BORDER_ID, 		AR_GAME_BORDER_RESET);
		task_post_pure_msg(AR_GAME_BUTTERFLY_ID,          AR_GAME_BUTTERFLY_RESET);
		task_post_pure_msg(AR_GAME_BOSS_ID,			AR_GAME_BOSS_RESET);
		task_post_pure_msg(AR_GAME_BOSS_BULLET_ID,  AR_GAME_BOSS_BULLET_RESET);
		// Save and reset Score
		ar_game_score_read(&gamescore);
		gamescore.score_now = ar_game_score;
		rank_ranking();
		ar_game_score_write(&gamescore);
		ar_game_score = 10;

		// State update
		ar_game_state = GAME_OFF;
		SCREEN_TRAN(scr_game_over_handle, &scr_game_over);
	} break;

	case AR_GAME_SCREEN_FLASH: {
		border_flash = 6;
	} break;

	default:
		break;
	}
}
