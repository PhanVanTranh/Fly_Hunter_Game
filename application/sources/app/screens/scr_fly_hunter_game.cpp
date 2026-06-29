#include "scr_fly_hunter_game.h"

#include <stdlib.h>

#include "screens.h"


/*****************************************************************************/
/* Variable Declaration - fly_hunter game screen */
/*****************************************************************************/
#define AR_GAME_COMMAND_TEXT_MAX_LEN		(10)
#define AR_GAME_HIGHSCORE_SCORE				(300)
#define AR_GAME_ARRAY_SIZE(arr)				(sizeof(arr) / sizeof((arr)[0]))

extern uint8_t border_flash;
uint8_t ar_game_state; 
ar_game_setting_t settingsetup;
static uint8_t ar_game_over_text_index = 0;

game_state_t game_state = GAME_STATE_NORMAL;

uint16_t warning_timer = 0;

uint16_t next_boss_score = 1000;

static const char* ar_game_over_text = " Too Bad!";
static const char* const ar_game_praise_text[] = {
    "Excellent",  // 9
    "Good Job!",  // 9
    "  Great!",   // 8
    " Amazing!",  // 9
    " Winner!"    // 8
};

static const char* const ar_game_blame_text[] = {
    " Too Bad!",  // 9
    " Try More",  // 9
    " Missed!",   // 8
    "  Oops!",    // 6
    " So Close"   // 9
};

static const char* ar_game_random_text(const char* const* text_list, uint8_t text_count) {
	return text_list[rand() % text_count];
}

static void ar_game_print_text_partial(const char* text, uint8_t max_chars) {
	if (text == NULL) return;
	for (uint8_t i = 0; i < max_chars && text[i] != '\0'; i++) {
		view_render.print(text[i]);
	}
}

/*****************************************************************************/
/* View - fly_hunter game screen*/
/*****************************************************************************/
void ar_game_frame_display() {
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	// view_render.setCursor(2,55);
	// view_render.print("Arrow:");
	// view_render.print(settingsetup.num_arrow);
	//view_render.setCursor(2,54);
	//view_render.print("Heart:");
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

void ar_game_meteoroid_display() {
	for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
		if (meteoroid[i].visible == WHITE) {
			if (meteoroid[i].action_image == AR_GAME_METEOROID_ACTION_IMAGE_1) {
				view_render.drawBitmap(	meteoroid[i].x, \
										meteoroid[i].y, \
										bitmap_fly_I, \
										SIZE_BITMAP_METEOROIDS_X, \
										SIZE_BITMAP_METEOROIDS_Y, \
				 						WHITE);
			}
			else if (meteoroid[i].action_image == AR_GAME_METEOROID_ACTION_IMAGE_2) {
				view_render.drawBitmap(	meteoroid[i].x, \
										meteoroid[i].y, \
										bitmap_fly_II, \
										SIZE_BITMAP_METEOROIDS_X, \
										SIZE_BITMAP_METEOROIDS_Y, \
										WHITE);
			}
			else if (meteoroid[i].action_image == AR_GAME_METEOROID_ACTION_IMAGE_3) {
				view_render.drawBitmap(	meteoroid[i].x, \
										meteoroid[i].y, \
										bitmap_fly_III, \
										SIZE_BITMAP_METEOROIDS_X, \
										SIZE_BITMAP_METEOROIDS_Y, \
				 						WHITE);
			}
		}
	}
}

void ar_game_bee_display() {

    for(uint8_t i = 0; i < NUM_BEE; i++) {

        if(bee[i].visible == WHITE) {

            switch(bee[i].action_image) {

            case AR_GAME_BEE_ACTION_IMAGE_1:

                view_render.drawBitmap(
                    bee[i].x,
                    bee[i].y,
                    bitmap_bee_I,
                    SIZE_BITMAP_BEE_X,
                    SIZE_BITMAP_BEE_Y,
                    WHITE);

                break;

            case AR_GAME_BEE_ACTION_IMAGE_2:

                view_render.drawBitmap(
                    bee[i].x,
                    bee[i].y,
                    bitmap_bee_II,
                    SIZE_BITMAP_BEE_X,
                    SIZE_BITMAP_BEE_Y,
                    WHITE);

                break;

            case AR_GAME_BEE_ACTION_IMAGE_3:

                view_render.drawBitmap(
                    bee[i].x,
                    bee[i].y,
                    bitmap_bee_III,
                    SIZE_BITMAP_BEE_X,
                    SIZE_BITMAP_BEE_Y,
                    WHITE);

                break;
            }
        }
    }
	for(uint8_t i=0;i<NUM_BEE;i++)
	{
		if(bee[i].show_minus20 &&
			(bee[i].minus20_timer % 2 == 0))
			{
				view_render.setCursor(
					bee[i].minus20_x,
					bee[i].minus20_y);

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

				for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
					view_render.fillCircle(
						border.x,
						meteoroid[i].y + 5,
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

			for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
				view_render.fillCircle(
					border.x,
					meteoroid[i].y + 3,
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

void ar_game_boss_bullet_display()
{
    for(uint8_t i=0;i<NUM_BOSS_BULLET;i++)
    {
        if(boss_bullet[i].visible == BLACK)
            continue;

        view_render.fillCircle(
            boss_bullet[i].x,
            boss_bullet[i].y,
            2,
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
		ar_game_meteoroid_display();
		ar_game_bee_display();
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
	else if (ar_game_state == GAME_OVER) {
		view_render.clear();
		view_render.drawBitmap(	0, \
			0, \
			bitmap_dolphin, \
			119, \
			62, \
			WHITE);
		
		view_render.setTextSize(1);
		view_render.setTextColor(WHITE);
		view_render.setCursor(74, 15);
		ar_game_print_text_partial(ar_game_over_text, ar_game_over_text_index);
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
		task_post_pure_msg(AR_GAME_METEOROID_ID, 	AR_GAME_METEOROID_SETUP);
		task_post_pure_msg(AR_GAME_BANG_ID, 	 	AR_GAME_BANG_SETUP);
		task_post_pure_msg(AR_GAME_BORDER_ID, 	 	AR_GAME_BORDER_SETUP);
		task_post_pure_msg(AR_GAME_BEE_ID,           AR_GAME_BEE_SETUP);
		task_post_pure_msg(AR_GAME_BOSS_ID,         AR_GAME_BOSS_SETUP);
		task_post_pure_msg(AR_GAME_BOSS_BULLET_ID,  AR_GAME_BOSS_BULLET_SETUP);
		// Set state 'GAME_PLAY' & remove idle screen timer
		ar_game_state = GAME_PLAY;
		game_state = GAME_STATE_NORMAL;

		warning_timer = 0;

		next_boss_score = 1000;

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
		task_post_pure_msg(AR_GAME_METEOROID_ID, 	AR_GAME_METEOROID_RUN);
		task_post_pure_msg(AR_GAME_METEOROID_ID, 	AR_GAME_METEOROID_DETONATOR);
		task_post_pure_msg(AR_GAME_BANG_ID, 		AR_GAME_BANG_UPDATE);
		task_post_pure_msg(AR_GAME_BORDER_ID, 		AR_GAME_LEVEL_UP);
		task_post_pure_msg(AR_GAME_BORDER_ID, 		AR_GAME_CHECK_GAME_OVER);
		task_post_pure_msg(AR_GAME_BEE_ID,           AR_GAME_BEE_RUN);
		task_post_pure_msg(AR_GAME_BEE_ID,           AR_GAME_BEE_DETONATOR);
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
		task_post_pure_msg(AR_GAME_METEOROID_ID,	AR_GAME_METEOROID_RESET);
		task_post_pure_msg(AR_GAME_BANG_ID, 		AR_GAME_BANG_RESET);
		task_post_pure_msg(AR_GAME_BORDER_ID, 		AR_GAME_BORDER_RESET);
		task_post_pure_msg(AR_GAME_BEE_ID,          AR_GAME_BEE_RESET);
		task_post_pure_msg(AR_GAME_BOSS_ID,			AR_GAME_BOSS_RESET);
		task_post_pure_msg(AR_GAME_BOSS_BULLET_ID,  AR_GAME_BOSS_BULLET_RESET);
		// Reset text animation index
		ar_game_over_text_index = 0;

		// Timer Exit
		timer_set(	AC_TASK_DISPLAY_ID, \
					AR_GAME_EXIT_GAME, \
					AR_GAME_TIME_EXIT_INTERVAL, \
					TIMER_ONE_SHOT);

		// Save and reset Score
		ar_game_score_read(&gamescore);
		gamescore.score_now = ar_game_score;
		rank_ranking();
		ar_game_score_write(&gamescore);
		ar_game_score = 10;

		// Setup text animation timer
		timer_set(	AC_TASK_DISPLAY_ID, \
					AR_GAME_OVER_TEXT_ANIM_TICK, \
					AR_GAME_OVER_TEXT_ANIM_TICK_INTERVAL, \
					TIMER_PERIODIC);

		if (gamescore.score_now > AR_GAME_HIGHSCORE_SCORE) {
			ar_game_over_text = ar_game_random_text(ar_game_praise_text, \
													AR_GAME_ARRAY_SIZE(ar_game_praise_text));
			BUZZER_PlaySound(BUZZER_SOUND_HIGHSCORE);
		}
		else {
			ar_game_over_text = ar_game_random_text(ar_game_blame_text, \
													AR_GAME_ARRAY_SIZE(ar_game_blame_text));
			BUZZER_PlaySound(BUZZER_SOUND_LOWSCORE);
		}

		// State update
		ar_game_state = GAME_OVER;
	} break;

	case AR_GAME_OVER_TEXT_ANIM_TICK: {
		APP_DBG_SIG("AR_GAME_OVER_TEXT_ANIM_TICK\n");
		if (ar_game_over_text_index < AR_GAME_COMMAND_TEXT_MAX_LEN) {
			ar_game_over_text_index++;
		}
		else {
			timer_remove_attr(AC_TASK_DISPLAY_ID, AR_GAME_OVER_TEXT_ANIM_TICK);
		}
	} break;

	case AR_GAME_EXIT_GAME: {
		APP_DBG_SIG("AR_GAME_EXIT_GAME\n");
		// Stop text animation timer
		timer_remove_attr(AC_TASK_DISPLAY_ID, AR_GAME_OVER_TEXT_ANIM_TICK);

		// Set state 'GAME_OFF' & set timer show idle screen
		ar_game_state = GAME_OFF;
		timer_set(	AC_TASK_DISPLAY_ID, \
			AC_DISPLAY_SHOW_IDLE, \
			AC_DISPLAY_IDLE_INTERVAL, \
			TIMER_ONE_SHOT);

		// Change the screen
		SCREEN_TRAN(scr_game_over_handle, &scr_game_over);
	} break;

	case AR_GAME_SCREEN_FLASH: {
		border_flash = 6;
	} break;

	default:
		break;
	}
}

