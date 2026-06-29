#include "ar_game_border.h"
#include "ar_game_meteoroid.h"
#include "ar_game_fly_hunter.h"

#define AR_GAME_LEVEL_UP_SCORE          (300)

ar_game_border_t border;
uint32_t ar_game_score					 = 10;
uint8_t player_life = PLAYER_MAX_LIFE;
static uint32_t ar_game_next_level_score = AR_GAME_LEVEL_UP_SCORE;

uint8_t border_flash = 0;  // Flash game border

void ar_game_border_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case AR_GAME_BORDER_SETUP: {
		APP_DBG_SIG("AR_GAME_BORDER_SETUP\n");
		border.x			= AXIS_X_BORDER;
		border.visible		= WHITE;
		border.action_image = AR_GAME_BORDER_ACTION_IMAGE_1;

		//ar_game_score = 0;
		player_life = PLAYER_MAX_LIFE;
	} break;

	case AR_GAME_LEVEL_UP: {
		APP_DBG_SIG("AR_GAME_LEVEL_UP\n");
		if (ar_game_score >= ar_game_next_level_score) {
			if (settingsetup.meteoroid_speed < AR_GAME_SETTING_METEOROID_SPEED_MAX) {
				settingsetup.meteoroid_speed++;
			}
			ar_game_next_level_score += AR_GAME_LEVEL_UP_SCORE;
		}
	} break;

	case AR_GAME_CHECK_GAME_OVER: {
		APP_DBG_SIG("AR_GAME_CHECK_GAME_OVER\n");
		// for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {
		// 	if (meteoroid[i].visible == WHITE && meteoroid[i].x <= (border.x - 3)) {
		// 		task_post_pure_msg(AR_GAME_SCREEN_ID, AR_GAME_RESET);
		// 		break;
		// 	}
		// }

		 for (uint8_t i = 0; i < NUM_METEOROIDS; i++) {

			if (meteoroid[i].visible == WHITE &&
				meteoroid[i].x <= (border.x - 3))
			{
				if (player_life > 0) {
					player_life--;
					BUZZER_PlaySound(BUZZER_SOUND_HIT);

					task_post_pure_msg(
					AR_GAME_SCREEN_ID,
					AR_GAME_SCREEN_FLASH
					);

					if(player_life == 0) {
						task_post_pure_msg(AR_GAME_SCREEN_ID, AR_GAME_RESET);
					}

					
				}

				if (player_life == 0) {
					task_post_pure_msg(
						AR_GAME_SCREEN_ID,
						AR_GAME_RESET
					);
				}
				else {
					meteoroid[i].x = RANDOM_METEOROID_X();

					if (i == 0) {
						meteoroid[i].y = RANDOM_METEOROID_Y_TOP();
					}
					else if (i == 1) {
						meteoroid[i].y = RANDOM_METEOROID_Y_MIDDLE();
					}
					else {
						meteoroid[i].y = RANDOM_METEOROID_Y_BOTTOM();
					}
					
					//meteoroid_adjust_position(i);
				}

				break;
			}
    	}
	} break;

	case AR_GAME_BORDER_RESET: {
		APP_DBG_SIG("AR_GAME_BORDER_RESET\n");
		border.visible = BLACK;
	} break;

	default:
		break;
	}
}
