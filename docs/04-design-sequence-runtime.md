# Runtime Signal Processing

This document describes the end-to-end runtime message flow of Fly Hunter, from power-on to Game Over. The gameplay is driven by the AK framework's task/timer/message scheduler — every game object operates as an independent task and communicates only through runtime signals (see [02-guide-coding-rule.md](./02-guide-coding-rule.md) for the architecture rules behind this).

## I. Boot and Menu Flow

```mermaid
sequenceDiagram
    autonumber
    participant Boot as Bootloader
    participant Startup as scr_startup
    participant Menu as scr_menu_game
    participant Setting as scr_game_setting
    participant Charts as scr_charts_game

    Boot->>Startup: jump to Application partition
    Startup->>Startup: SCREEN_ENTRY -> show logo (bitmap_fly_hunter_logo)
    Startup->>Menu: SCREEN_TRAN (after timeout / button press)

    Menu->>Menu: SCREEN_ENTRY -> draw Start Game / Settings / High Score / Exit

    alt Select "Settings"
        Menu->>Setting: SCREEN_TRAN(scr_game_setting_handle)
        Setting->>Setting: SCREEN_ENTRY -> ar_game_setting_read()
        Note over Setting: adjust ARROWS / FLY SPEED / SOUND with UP/DOWN/MODE
        Setting->>Setting: ar_game_setting_write() on each change
        Setting->>Menu: SCREEN_TRAN back (EXIT)
    else Select "High Score"
        Menu->>Charts: SCREEN_TRAN(scr_charts_game_handle)
        Charts->>Charts: SCREEN_ENTRY -> ar_game_score_read() -> show Top 3
        Charts->>Menu: SCREEN_TRAN back
    else Select "Start Game"
        Menu->>Menu: SCREEN_TRAN(scr_fly_hunter_game_handle) — see section II
    end
```

## II. Game Start — Object Initialization

```mermaid
sequenceDiagram
    autonumber
    actor Player
    participant Menu as scr_menu_game
    participant Screen as scr_fly_hunter_game
    participant Hunter
    participant Arrow
    participant Fly
    participant Butterfly
    participant Boss
    participant BossBullet as Boss Bullet
    participant Bang
    participant Border

    Player->>Menu: press MODE on "Start Game"
    Menu->>Screen: SCREEN_TRAN(scr_fly_hunter_game_handle)
    activate Screen
    Screen->>Screen: SCREEN_ENTRY -> ar_game_setting_read(&settingsetup)

    Screen->>Hunter: AR_GAME_FLY_HUNTER_SETUP
    Screen->>Arrow: AR_GAME_ARROW_SETUP
    Screen->>Fly: AR_GAME_FLY_SETUP
    Screen->>Butterfly: AR_GAME_BUTTERFLY_SETUP
    Screen->>Boss: AR_GAME_BOSS_SETUP
    Screen->>BossBullet: AR_GAME_BOSS_BULLET_SETUP
    Screen->>Bang: AR_GAME_BANG_SETUP
    Screen->>Border: AR_GAME_BORDER_SETUP

    Note over Screen: ar_game_state = GAME_PLAY<br/>game_state = GAME_STATE_NORMAL<br/>next_boss_score = 500
    Screen->>Screen: timer_set(AR_GAME_TIME_TICK, PERIODIC)
    deactivate Screen
```

## III. Main Gameplay Loop

```mermaid
sequenceDiagram
    autonumber
    participant Timer as AR_GAME_TIME_TICK
    participant Screen as scr_fly_hunter_game
    participant Hunter
    participant Arrow
    participant Fly
    participant Butterfly
    participant Boss
    participant BossBullet as Boss Bullet
    participant Border

    Timer->>Screen: AR_GAME_TIME_TICK
    activate Screen

    Screen->>Hunter: AR_GAME_FLY_HUNTER_UPDATE
    Screen->>Arrow: AR_GAME_ARROW_RUN
    Screen->>Fly: AR_GAME_FLY_RUN
    Screen->>Fly: AR_GAME_FLY_DETONATOR
    Screen->>Butterfly: AR_GAME_BUTTERFLY_RUN
    Screen->>Butterfly: AR_GAME_BUTTERFLY_DETONATOR
    Screen->>Boss: AR_GAME_BOSS_RUN
    Screen->>Boss: AR_GAME_BOSS_DETONATOR
    Screen->>BossBullet: AR_GAME_BOSS_BULLET_RUN

    Screen->>Border: AR_GAME_LEVEL_UP
    Note right of Border: score >= next_level_score (300, 600, ...)<br/>-> fly_speed++, next_level_score += 300

    Screen->>Border: AR_GAME_CHECK_GAME_OVER
    Note right of Border: a Fly reached the Border<br/>-> life--, or post AR_GAME_RESET if life == 0

    alt score >= next_boss_score (500, 1000, ...)
        Note over Screen: game_state = GAME_STATE_WARNING<br/>warning_timer = 40<br/>flash "WARNING - BOSS INCOMING" + 3-beep
    end

    alt warning_timer reaches 0
        Note over Screen: game_state = GAME_STATE_BOSS
        Screen->>Boss: AR_GAME_BOSS_SPAWN
    end

    deactivate Screen
```

## IV. Boss Encounter and Reward Loop

```mermaid
sequenceDiagram
    autonumber
    participant Screen as scr_fly_hunter_game
    participant Boss
    participant BossBullet as Boss Bullet
    participant Arrow
    participant Hunter

    Screen->>Boss: AR_GAME_BOSS_SPAWN
    activate Boss
    Note right of Boss: visible, clears Fly/Butterfly/Arrow, refills arrows
    Boss->>BossBullet: AR_GAME_BOSS_BULLET_SETUP + schedule first fire
    deactivate Boss

    loop while Boss alive
        Arrow-->>Boss: arrow hit -> hp--, hit_flash, shake
        BossBullet-->>Hunter: bullet hit -> life--
        alt life == 0
            Note over Screen: post AR_GAME_RESET (once) -> Game Over
        end
    end

    alt Boss hp reaches 0
        Note right of Boss: state = BOSS_STATE_DYING<br/>3-frame explosion animation
        Boss->>BossBullet: AR_GAME_BOSS_BULLET_RESET
        Note over Screen: +300 score, +1 life (capped),<br/>next_boss_score += 500, boss_hp += 5<br/>game_state = GAME_STATE_NORMAL<br/>respawn Fly + Butterfly
    else Boss reaches the Border
        Note over Screen: life--, hide Boss, reset Bullet<br/>if life == 0: post AR_GAME_RESET (once)<br/>else: game_state = GAME_STATE_NORMAL
    end
```

## V. Game Over Transition

This is the most bug-sensitive part of the runtime flow — see [02-guide-coding-rule.md](./02-guide-coding-rule.md) for the rules that keep it correct.

```mermaid
sequenceDiagram
    autonumber
    participant Border
    participant Boss
    participant BossBullet as Boss Bullet
    participant Screen as scr_fly_hunter_game
    participant GameOver as scr_game_over

    alt life reaches 0 (from Border, Boss, or Boss Bullet — exactly one path fires)
        Note over Screen: post AR_GAME_RESET (once)
    end

    Screen->>Screen: AR_GAME_RESET handler
    activate Screen
    Screen->>Screen: timer_remove_attr(AR_GAME_TIME_TICK)
    Note over Screen: post *_RESET to every object<br/>(Hunter, Arrow, Fly, Butterfly, Boss, Boss Bullet, Bang, Border)
    Screen->>Screen: ar_game_score_read(&gamescore)<br/>gamescore.score_now = ar_game_score<br/>rank_ranking()<br/>ar_game_score_write(&gamescore)
    Screen->>Screen: ar_game_state = GAME_OFF
    Screen->>GameOver: SCREEN_TRAN(scr_game_over_handle, &scr_game_over)
    deactivate Screen

    GameOver->>GameOver: SCREEN_ENTRY -> view_scr_game_over() -> view_render.update()
    Note over GameOver: shows "GAME OVER" + score + Restart / Chart / Home icons immediately
```

## VI. From Game Over

```mermaid
sequenceDiagram
    autonumber
    actor Player
    participant GameOver as scr_game_over
    participant Menu as scr_menu_game
    participant Charts as scr_charts_game
    participant Screen as scr_fly_hunter_game
    participant Idle as scr_idle

    alt Player presses MODE
        Player->>GameOver: MODE
        GameOver->>Menu: SCREEN_TRAN(scr_menu_game_handle)
    else Player presses UP
        Player->>GameOver: UP
        GameOver->>Charts: SCREEN_TRAN(scr_charts_game_handle)
    else Player presses DOWN
        Player->>GameOver: DOWN
        GameOver->>Screen: SCREEN_TRAN(scr_fly_hunter_game_handle) — play again
    else No input for AC_DISPLAY_IDLE_INTERVAL
        GameOver->>Idle: SCREEN_TRAN(scr_idle_handle)<br/>scr_idle_set_return_screen(scr_game_over_handle, &scr_game_over)
    end
```

## VII. Code References

| Stage | Source file |
|---|---|
| Boot / Menu | `scr_startup.cpp`, `scr_menu_game.cpp` |
| Settings | `scr_game_setting.cpp` |
| Gameplay orchestration | `scr_fly_hunter_game.cpp` |
| Game Over | `scr_game_over.cpp` |
| High Score | `scr_charts_game.cpp` |
| Idle timeout | `scr_idle.cpp` |
| Per-object logic | `application/sources/app/game/fly_hunter_game/*.cpp` (see [03-design-sequence-object.md](./03-design-sequence-object.md)) |
