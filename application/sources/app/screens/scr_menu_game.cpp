#include "scr_menu_game.h"

/*****************************************************************************/
/* MENU CONFIGURATION                                                       */
/*****************************************************************************/

#define NUMBER_MENU_ITEMS             (4)

#define MENU_TITLE_X                  (10)
#define MENU_TITLE_Y                  (0)

#define MENU_LOGO_X                   (52)
#define MENU_LOGO_Y                   (16)

#define MENU_LOGO_W                   (25)
#define MENU_LOGO_H                   (25)

#define MENU_ITEM_X                   (10)
#define MENU_ITEM_Y                   (18)

#define MENU_ITEM_HEIGHT              (11)

#define MENU_SELECT_X                 (2)
#define MENU_SELECT_W                 (124)
#define MENU_SELECT_H                 (10)

/*****************************************************************************/
/* MENU VARIABLES                                                           */
/*****************************************************************************/

static uint8_t menu_index = 0;

/*****************************************************************************/
/* MENU TEXT                                                                */
/*****************************************************************************/

static char menu_items_name[NUMBER_MENU_ITEMS][20] =
{
    "START GAME",
    "SETTINGS",
    "HIGH SCORE",
    "EXIT"
};

/*****************************************************************************/
/* FUNCTION DECLARATION                                                     */
/*****************************************************************************/

static void view_scr_menu_game(void);
static void update_menu_screen_chosse(void);
static void screen_tran_menu(void);

/*****************************************************************************/
/* VIEW                                                                     */
/*****************************************************************************/

view_dynamic_t dyn_view_menu =
{
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_menu_game
};

view_screen_t scr_menu_game =
{
    &dyn_view_menu,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

/*****************************************************************************/
/* VIEW - MENU                                                              */
/*****************************************************************************/

static void view_scr_menu_game(void)
{
    view_render.clear();

    /**********************************************************************/
    /* Title                                                              */
    /**********************************************************************/

    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);

    view_render.setCursor(MENU_TITLE_X, MENU_TITLE_Y);
    view_render.print("FLY");

    view_render.setCursor(56, MENU_TITLE_Y);
    view_render.print("HUNTER");

    /**********************************************************************/
    /* Menu                                                               */
    /**********************************************************************/

    view_render.setTextSize(1);

    for(uint8_t i = 0; i < NUMBER_MENU_ITEMS; i++)
    {
        uint8_t y = MENU_ITEM_Y + i * MENU_ITEM_HEIGHT;

        /**************************************************************/
        /* Selected Item                         */
        /**************************************************************/

        if(i == menu_index)
        {
            view_render.fillRoundRect(
                MENU_SELECT_X,
                y - 2,
                MENU_SELECT_W,
                MENU_SELECT_H,
                2,
                WHITE
            );

            view_render.setTextColor(BLACK);
            view_render.setCursor(MENU_ITEM_X, y);
            view_render.print("> ");
            view_render.print(menu_items_name[i]);
        }

        /**************************************************************/
        /* Normal Item                              */
        /**************************************************************/

        else
        {
            view_render.setTextColor(WHITE);
            view_render.setCursor(MENU_ITEM_X + 8, y);
            view_render.print(menu_items_name[i]);
        }
    }


    view_render.update();
}

/*****************************************************************************/
/* SCREEN TRANSITION                                                        */
/*****************************************************************************/

static void screen_tran_menu(void)
{
    switch(menu_index)
    {
        case 0:
            SCREEN_TRAN(scr_fly_hunter_game_handle, &scr_fly_hunter_game);
            break;

        case 1:
            SCREEN_TRAN(scr_game_setting_handle, &scr_game_setting);
            break;

        case 2:
            SCREEN_TRAN(scr_charts_game_handle, &scr_charts_game);
            break;

        case 3:
            scr_idle_set_return_screen(scr_menu_game_handle, &scr_menu_game);
            SCREEN_TRAN(scr_idle_handle, &scr_idle);
            break;

        default:
            break;
    }
}

/*****************************************************************************/
/* UPDATE MENU                                                              */
/*****************************************************************************/

static void update_menu_screen_chosse(void)
{
    view_scr_menu_game();
}

/*****************************************************************************/
/* HANDLE MENU                                                              */
/*****************************************************************************/

void scr_menu_game_handle(ak_msg_t* msg)
{
    switch(msg->sig)
    {
        case SCREEN_ENTRY:
        {
            APP_DBG_SIG("SCREEN_ENTRY\n");
            menu_index = 0;
            update_menu_screen_chosse();

            timer_set(
                AC_TASK_DISPLAY_ID,
                AC_DISPLAY_SHOW_IDLE,
                AC_DISPLAY_IDLE_INTERVAL,
                TIMER_ONE_SHOT
            );
        }
        break;

        case AC_DISPLAY_SHOW_IDLE:
        {
            APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE\n");
            timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);

            scr_idle_set_return_screen(scr_menu_game_handle, &scr_menu_game);
            SCREEN_TRAN(scr_idle_handle, &scr_idle);
        }
        break;

        case AC_DISPLAY_BUTTON_UP_PRESSED:
        {
            APP_DBG_SIG("BUTTON_UP\n");

            timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
            timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);

            if(menu_index > 0)
            {
                menu_index--;
            }
            else
            {
                menu_index = NUMBER_MENU_ITEMS - 1; 
            }

            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
            update_menu_screen_chosse();
        }
        break;

        case AC_DISPLAY_BUTTON_DOWN_PRESSED:
        {
            APP_DBG_SIG("BUTTON_DOWN\n");
           
            timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
            timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);

            if(menu_index < (NUMBER_MENU_ITEMS - 1))
            {
                menu_index++;
            }
            else
            {
                menu_index = 0; 
            }

            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
            update_menu_screen_chosse();
        }
        break;

        case AC_DISPLAY_BUTTON_MODE_PRESSED:
        {
            APP_DBG_SIG("BUTTON_MODE\n");
            
            timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);

            BUZZER_PlaySound(BUZZER_SOUND_CLICK); 
            screen_tran_menu();
        }
        break;

        default:
            break;
    }
}
