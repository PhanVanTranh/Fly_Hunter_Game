#include "scr_startup.h"

#define       SIZE_BITMAP_FLY_HUNTER_LOGO_X 40
#define       SIZE_BITMAP_FLY_HUNTER_LOGO_Y 40

static uint8_t startup_state = 0;

enum
{
    STARTUP_AK = 0,
    STARTUP_FLY_HUNTER
};

#define STARTUP_AK_TIME             800     
#define STARTUP_GAME_LOGO_TIME      1500     

/*****************************************************************************/
/* View - startup */
/*****************************************************************************/
static void view_scr_startup();
static void view_startup_ak();
static void view_startup_fly_hunter();

view_dynamic_t dyn_view_startup = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_startup
};

view_screen_t scr_startup = {
	&dyn_view_startup,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_startup()
{
    view_render.clear();

    if(startup_state == STARTUP_AK)
    {
        view_startup_ak();
    }
    else
    {
        view_startup_fly_hunter();
    }

    view_render.update();
}

void view_startup_ak()
{
	#define AK_LOGO_AXIS_X		(23)
	#define AK_LOGO_TEXT		(AK_LOGO_AXIS_X + 4)
	/* ak logo */
	//BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
	//view_render.clear();
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(AK_LOGO_AXIS_X, 3);
	view_render.print("   __    _  _ ");
	view_render.setCursor(AK_LOGO_AXIS_X, 10);
	view_render.print("  /__\\  ( )/ )");
	view_render.setCursor(AK_LOGO_AXIS_X, 20);
	view_render.print(" /(__)\\ (   (");
	view_render.setCursor(AK_LOGO_AXIS_X, 30);
	view_render.print("(__)(__)(_)\\_)");
	view_render.setCursor(AK_LOGO_TEXT, 42);
	view_render.print("Active Kernel");
	//view_render.update();
}

void view_startup_fly_hunter()
{

    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);

    view_render.setCursor(35,2);
    view_render.print("FLY HUNTER");

    //------------------------------------
    // GAME LOGO
    //------------------------------------

    view_render.drawBitmap(
        44,
        12,
        bitmap_fly_hunter_logo,
        SIZE_BITMAP_FLY_HUNTER_LOGO_X,
        SIZE_BITMAP_FLY_HUNTER_LOGO_Y,
        WHITE);

    view_render.setCursor(30,56);
    view_render.print("FPT Telecom");
}

/*****************************************************************************/
/* Handle - startup */
/*****************************************************************************/
void scr_startup_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case AC_DISPLAY_INITIAL:
	{
		APP_DBG_SIG("AC_DISPLAY_INITIAL\n");

		startup_state = STARTUP_AK;

		view_render.initialize();

		view_render_display_on();

		ar_game_setting_read(&settingdata);

		BUZZER_Sleep(settingdata.silent);

		BUZZER_PlaySound(BUZZER_SOUND_STARTUP);

		view_scr_startup();


		timer_set(

			AC_TASK_DISPLAY_ID,

			AC_DISPLAY_SHOW_STARTUP_LOGO,

			STARTUP_AK_TIME,

			TIMER_ONE_SHOT);
		}
	break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");
		SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);
	} break;

	case AC_DISPLAY_SHOW_STARTUP_LOGO: {
		 APP_DBG_SIG("AC_DISPLAY_SHOW_STARTUP_LOGO\n");
		BUZZER_PlaySound(BUZZER_SOUND_STARTUP_GAME);

		startup_state = STARTUP_FLY_HUNTER;

		view_scr_startup();

		timer_set(

			AC_TASK_DISPLAY_ID,

			AC_DISPLAY_SHOW_GAME_LOGO,

			STARTUP_GAME_LOGO_TIME,

			TIMER_ONE_SHOT);
	} break;

	case AC_DISPLAY_SHOW_GAME_LOGO:
	{
		APP_DBG_SIG("AC_DISPLAY_SHOW_GAME_LOGO\n");

		SCREEN_TRAN(scr_menu_game_handle,&scr_menu_game);

		
	}
	break;

	default:
		break;

	}
}
