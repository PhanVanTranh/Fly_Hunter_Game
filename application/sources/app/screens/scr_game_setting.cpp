#include "scr_game_setting.h"

/*****************************************************************************/
/* Variable Declaration - Setting game */
/*****************************************************************************/
static uint8_t setting_location_chosse;

/*****************************************************************************/
/* View - Setting game */
/*****************************************************************************/
static void view_scr_game_setting();

view_dynamic_t dyn_view_item_game_setting = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game_setting
};

view_screen_t scr_game_setting = {
	&dyn_view_item_game_setting,
	ITEM_NULL,
	ITEM_NULL,
	.focus_item = 0,
};

void view_scr_game_setting() {
	view_render.clear();

	// Left chevron arrow
	view_render.fillTriangle(6, 6, 12, 2, 12, 10, WHITE);
	view_render.drawFastVLine(4, 2, 9, WHITE);
	
	// SETTINGS frame border
	view_render.drawRoundRect(18, 1, 92, 12, 2, WHITE);
	
	// Removed bitmap gear drawing command here
	
	// Adjusted X coordinate from 38 to 43 so SETTINGS text is centered in the frame
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(43, 3); 
	view_render.print("SETTINGS");
	
	// Right chevron arrow
	view_render.fillTriangle(122, 6, 116, 2, 116, 10, WHITE);
	view_render.drawFastVLine(124, 2, 9, WHITE);


	view_render.drawFastHLine(2, 15, 124, WHITE);
	view_render.drawFastHLine(2, 27, 124, WHITE);
	view_render.drawFastHLine(2, 39, 124, WHITE);
	view_render.drawFastHLine(2, 51, 124, WHITE);
	view_render.drawFastHLine(2, 63, 124, WHITE);

	uint8_t highlight_y = 0;
	switch (setting_location_chosse) {
		case SETTING_ITEM_ARRDESS_1: highlight_y = 16; break;
		case SETTING_ITEM_ARRDESS_2: highlight_y = 28; break;
		case SETTING_ITEM_ARRDESS_3: highlight_y = 40; break;
		case SETTING_ITEM_ARRDESS_4: highlight_y = 52; break;
	}

	view_render.fillRoundRect(2, highlight_y, 124, 11, 2, WHITE);
	view_render.drawRoundRect(3, highlight_y + 1, 122, 9, 1, BLACK);

	
	// --- ARROWS ---
	uint8_t color_0 = (setting_location_chosse == SETTING_ITEM_ARRDESS_1) ? BLACK : WHITE;
	view_render.drawBitmap(6, 15, icon_arrows, CYBER_ICON_W, CYBER_ICON_H, color_0);
	view_render.setTextColor(color_0);
	view_render.setCursor(24, 17);
	view_render.print("ARROWS");
	view_render.setCursor(114, 17);
	view_render.print(settingdata.num_arrow);

	// ---  FLY SPEED ---
	uint8_t color_1 = (setting_location_chosse == SETTING_ITEM_ARRDESS_2) ? BLACK : WHITE;
	view_render.drawBitmap(6, 27, icon_fly, CYBER_ICON_W, CYBER_ICON_H, color_1);
	view_render.setTextColor(color_1);
	view_render.setCursor(24, 29);
	view_render.print("FLY SPEED");
	view_render.setCursor(114, 29);
	view_render.print(settingdata.fly_speed);

	// ---  SOUND ---
	uint8_t color_2 = (setting_location_chosse == SETTING_ITEM_ARRDESS_3) ? BLACK : WHITE;
	view_render.drawBitmap(6, 39, icon_sound, CYBER_ICON_W, CYBER_ICON_H, color_2);
	view_render.setTextColor(color_2);
	view_render.setCursor(24, 41);
	view_render.print("SOUND");
	view_render.setCursor(108, 41);
	if (settingdata.silent == AR_GAME_SETTING_SILENT_OFF) {
		view_render.print(" ON");
	} else {
		view_render.print("OFF");
	}

	// ---  EXIT ---
	uint8_t color_3 = (setting_location_chosse == SETTING_ITEM_ARRDESS_4) ? BLACK : WHITE;
	view_render.drawBitmap(6, 51, icon_exit, CYBER_ICON_W, CYBER_ICON_H, color_3);
	view_render.setTextColor(color_3);
	view_render.setCursor(24, 53);
	view_render.print("EXIT");

	view_render.update();
}

/*****************************************************************************/
/* Handle - Setting game */
/*****************************************************************************/
void scr_game_setting_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		view_render.clear();
		setting_location_chosse = SETTING_ITEM_ARRDESS_1;
		ar_game_setting_read(&settingdata);
	} break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");
		switch (setting_location_chosse) {
		case SETTING_ITEM_ARRDESS_1:
			settingdata.num_arrow++;
			if (settingdata.num_arrow > AR_GAME_SETTING_NUM_ARROW_MAX) {
				settingdata.num_arrow = AR_GAME_SETTING_NUM_ARROW_MIN;
			}
			break;

		case SETTING_ITEM_ARRDESS_2:
			settingdata.fly_speed++;
			if (settingdata.fly_speed > AR_GAME_SETTING_FLY_SPEED_MAX) { 
				settingdata.fly_speed = AR_GAME_SETTING_FLY_SPEED_MIN;
			}
			break;

		case SETTING_ITEM_ARRDESS_3:
			settingdata.silent = !settingdata.silent;
			BUZZER_Sleep(settingdata.silent);
			break;

		case SETTING_ITEM_ARRDESS_4:
			settingdata.arrow_speed = AR_GAME_SETTING_ARROW_SPEED_DEFAULT;
			ar_game_setting_write(&settingdata);
			SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
			break;

		default: 
			break;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		view_scr_game_setting(); 
	} break;
	
	case AC_DISPLAY_BUTTON_UP_LONG_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_LONG_PRESSED\n");
		settingdata.num_arrow = AR_GAME_SETTING_NUM_ARROW_MAX;
		settingdata.fly_speed = AR_GAME_SETTING_FLY_SPEED_MAX;
		settingdata.silent = AR_GAME_SETTING_SILENT_OFF;
		
		BUZZER_Sleep(settingdata.silent);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		view_scr_game_setting();
	} break;

	case AC_DISPLAY_BUTTON_UP_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_PRESSED\n");
		if (setting_location_chosse > SETTING_ITEM_ARRDESS_1) {
			setting_location_chosse -= STEP_SETTING_CHOSSE;
		} else {
			setting_location_chosse = SETTING_ITEM_ARRDESS_4; 
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		view_scr_game_setting();
	} break;

	case AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED\n");
		settingdata.num_arrow = AR_GAME_SETTING_NUM_ARROW_MIN;
		settingdata.fly_speed = AR_GAME_SETTING_FLY_SPEED_MIN;
		settingdata.silent = AR_GAME_SETTING_SILENT_ON;
		
		BUZZER_Sleep(settingdata.silent);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		view_scr_game_setting();
	} break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_PRESSED\n");
		if (setting_location_chosse < SETTING_ITEM_ARRDESS_4) {
			setting_location_chosse += STEP_SETTING_CHOSSE;
		} else {
			setting_location_chosse = SETTING_ITEM_ARRDESS_1; 
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		view_scr_game_setting();
	} break;

	case AC_DISPLAY_SHOW_IDLE: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
		scr_idle_set_return_screen(scr_game_setting_handle, &scr_game_setting);
		SCREEN_TRAN(scr_idle_handle, &scr_idle);
	} break;

	default:
		break;
	}
}