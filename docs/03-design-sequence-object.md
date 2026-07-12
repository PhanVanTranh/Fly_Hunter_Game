# Game Object Sequences

This document describes the runtime sequence of each game object in Fly Hunter. Each object is handled by its own AK task and receives signals from the screen task (`scr_fly_hunter_game.cpp`), button callbacks, timers, or other object tasks.

## I. Object Summary

| Object | Task ID | Handler | Source file | Main responsibility |
| --- | --- | --- | --- | --- |
| Hunter | `AR_GAME_FLY_HUNTER_ID` | `ar_game_fly_hunter_handle()` | `ar_game_fly_hunter.cpp` | Controls the player's vertical position and idle/shoot sprite. |
| Arrow | `AR_GAME_ARROW_ID` | `ar_game_arrow_handle()` | `ar_game_arrow.cpp` | Shoots arrows, moves active arrows, restores available arrow count. |
| Fly | `AR_GAME_FLY_ID` | `ar_game_fly_handle()` | `ar_game_fly.cpp` | Spawns Flies, moves them, checks collision with arrows, awards score. |
| Butterfly | `AR_GAME_BUTTERFLY_ID` | `ar_game_butterfly_handle()` | `ar_game_butterfly.cpp` | Spawns a decoy Butterfly on a sine-wave path; penalizes score if shot. |
| Boss | `AR_GAME_BOSS_ID` | `ar_game_boss_handle()` | `ar_game_boss.cpp` | Boss spawn/movement/HP/death, and the extra-life reward on defeat. |
| Boss Bullet | `AR_GAME_BOSS_BULLET_ID` | `ar_game_boss_bullet_handle()` | `ar_game_boss_bullet.cpp` | Fires bullets from the Boss toward the Hunter and checks impact. |
| Bang | `AR_GAME_BANG_ID` | `ar_game_bang_handle()` | `ar_game_bang.cpp` | Plays the explosion animation after a Fly is hit. |
| Border | `AR_GAME_BORDER_ID` | `ar_game_border_handle()` | `ar_game_border.cpp` | Checks level-up and game-over conditions for Flies. |

**Note:** there are **three independent paths** that can cost the player a life and post `AR_GAME_RESET`: a Fly reaching the Border (`ar_game_border.cpp`), the Boss itself reaching the Border (`ar_game_boss.cpp`), and a Boss Bullet hitting the Hunter (`ar_game_boss_bullet.cpp`). Each path posts `AR_GAME_RESET` **only once** per life-loss event — keep it that way (see [02-guide-coding-rule.md](./02-guide-coding-rule.md)) since Fly and Boss never move at the same time (`game_state` gates them), so only one path can be active per tick in practice.

## II. Hunter Object Sequence

The Hunter owns the player's vertical position. `AR_GAME_FLY_HUNTER_UP` / `_DOWN` update an internal `fly_hunter_y` target; the next `AR_GAME_FLY_HUNTER_UPDATE` (posted every game tick) copies it into the rendered `fly_hunter.y`.

```mermaid
sequenceDiagram
    autonumber
    participant Button as Button Callback
    participant Timer as Game Tick Timer(AR_GAME_TIME_TICK)
    participant Screen as scr_fly_hunter_game
    participant Hunter as Hunter Task

    Screen->>Hunter: AR_GAME_FLY_HUNTER_SETUP
    activate Hunter
    Note right of Hunter: x, y = AXIS_X/Y_FLY_HUNTERaction_image based on num_arrow
    deactivate Hunter

    Button->>Hunter: AR_GAME_FLY_HUNTER_UP
    activate Hunter
    Note right of Hunter: fly_hunter_y -= STEPclamp to AXIS_Y_FLY_HUNTER_MIN (10)
    deactivate Hunter

    Timer->>Screen: AR_GAME_TIME_TICK
    Screen->>Hunter: AR_GAME_FLY_HUNTER_UPDATE
    activate Hunter
    Note right of Hunter: fly_hunter.y = fly_hunter_y
    deactivate Hunter

    Button->>Hunter: AR_GAME_FLY_HUNTER_DOWN
    activate Hunter
    Note right of Hunter: fly_hunter_y += STEPclamp to AXIS_Y_FLY_HUNTER_MAX (50)
    deactivate Hunter

    Screen->>Hunter: AR_GAME_FLY_HUNTER_RESET
    activate Hunter
    Note right of Hunter: visible = BLACK
    deactivate Hunter
```

## III. Arrow Object Sequence

Arrow receives shoot input from the MODE button (`AR_GAME_ARROW_SHOOT`). Every game tick, `AR_GAME_ARROW_RUN` moves visible arrows to the right; an arrow reaching the far edge is hidden and its slot returned to `settingsetup.num_arrow`.

```mermaid
sequenceDiagram
    autonumber
    participant Button as Button Callback (MODE)
    participant Timer as Game Tick Timer
    participant Screen as scr_fly_hunter_game
    participant Arrow as Arrow Task
    participant Hunter as Hunter State
    participant Buzzer as Buzzer

    Screen->>Arrow: AR_GAME_ARROW_SETUP
    activate Arrow
    Note right of Arrow: clear all arrow slots
    deactivate Arrow

    Button->>Arrow: AR_GAME_ARROW_SHOOT
    activate Arrow
    alt settingsetup.num_arrow == 0
        Arrow->>Buzzer: BUZZER_SOUND_3BEEP
    else arrow available
        Note right of Arrow: show arrow at hunter.y - 5num_arrow--
        Arrow->>Hunter: set "empty" sprite if num_arrow reaches 0
        Arrow->>Buzzer: BUZZER_SOUND_CLICK
    end
    deactivate Arrow

    Timer->>Screen: AR_GAME_TIME_TICK
    Screen->>Arrow: AR_GAME_ARROW_RUN
    activate Arrow
    Note right of Arrow: x += settingsetup.arrow_speed
    alt arrow reaches MAX_AXIS_X_ARROW
        Note right of Arrow: hide arrow, x = 0num_arrow++, restore "ready" sprite
    end
    deactivate Arrow

    Screen->>Arrow: AR_GAME_ARROW_RESET
    activate Arrow
    Note right of Arrow: hide all arrows
    deactivate Arrow
```

> **Note:** `arrow_speed` is stored in `ar_game_setting_t` but is not exposed on the Settings screen — it is fixed at `AR_GAME_SETTING_ARROW_SPEED_DEFAULT` (5), because a slow arrow speed would significantly hurt the gameplay feel.

## IV. Fly Object Sequence

Fly is the main scoring target. On each tick, the screen posts `AR_GAME_FLY_RUN` (move + animate) then `AR_GAME_FLY_DETONATOR` (collision check against arrows).

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer
    participant Screen as scr_fly_hunter_game
    participant Fly as Fly Task
    participant Arrow as Arrow State
    participant Bang as Bang State
    participant Border as Border State

    Screen->>Fly: AR_GAME_FLY_SETUP
    activate Fly
    Note right of Fly: 3 lanes (top/middle/bottom)random position, avoids Butterfly overlap
    deactivate Fly

    Timer->>Screen: AR_GAME_TIME_TICK
    Screen->>Fly: AR_GAME_FLY_RUN
    activate Fly
    Note right of Fly: only runs while game_state == GAME_STATE_NORMALx -= settingsetup.fly_speedadvance action_image
    deactivate Fly

    Screen->>Fly: AR_GAME_FLY_DETONATOR
    activate Fly
    Fly-->>Arrow: check visible arrows vs visible Flies
    alt arrow hits Fly
        Note right of Fly: hide Fly + arrowrespawn Fly at a new lane
        Fly->>Bang: show explosion at hit position
        Note right of Fly: score += 10num_arrow++ (if below max)
        alt score >= next_boss_score
            Note right of Fly: game_state = GAME_STATE_WARNINGwarning_timer = 90next_boss_score += 500
        end
    end
    deactivate Fly

    Screen->>Border: AR_GAME_CHECK_GAME_OVER (separate message, see section VIII)

    Screen->>Fly: AR_GAME_FLY_RESET
    activate Fly
    Note right of Fly: hide all Flies
    deactivate Fly
```

## V. Butterfly Object Sequence

Butterfly is a decoy: it flies in from the right on a sine-wave path (`butterfly_sin_table`) and should be avoided, not shot.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer
    participant Screen as scr_fly_hunter_game
    participant Butterfly as Butterfly Task
    participant Arrow as Arrow State
    participant Buzzer as Buzzer

    Screen->>Butterfly: AR_GAME_BUTTERFLY_SETUP
    activate Butterfly
    Note right of Butterfly: hidden, spawn_delay = random 1-5s
    deactivate Butterfly

    Timer->>Screen: AR_GAME_TIME_TICK
    Screen->>Butterfly: AR_GAME_BUTTERFLY_RUN
    activate Butterfly
    Note right of Butterfly: only runs while game_state == GAME_STATE_NORMAL
    alt hidden, spawn_delay elapsed
        Note right of Butterfly: pick position avoiding Fly overlapbecome visible
    else visible
        Note right of Butterfly: x -= fly_speedy follows sine-wave table
        alt flew past the Border undetected
            Note right of Butterfly: hide, schedule next spawn(no penalty — player dodged it)
        end
    end
    deactivate Butterfly

    Screen->>Butterfly: AR_GAME_BUTTERFLY_DETONATOR
    activate Butterfly
    Butterfly-->>Arrow: check visible arrows vs visible Butterfly
    alt arrow hits Butterfly
        Note right of Butterfly: hide Butterfly + arrowshow floating "-20" textschedule next spawn
        Note right of Butterfly: score -= 20 (floored at 0)
        Butterfly->>Buzzer: BUZZER_SOUND_BEE
    end
    deactivate Butterfly

    Screen->>Butterfly: AR_GAME_BUTTERFLY_RESET
    activate Butterfly
    Note right of Butterfly: hide, spawn_delay = 0
    deactivate Butterfly
```

## VI. Boss Object Sequence

The Boss only becomes active once `game_state` reaches `GAME_STATE_BOSS` (after the `GAME_STATE_WARNING` countdown finishes in `scr_fly_hunter_game.cpp`). It clears all Flies/Butterflies/Arrows on spawn, and rewards the player with **+300 score** and up to **+1 life** on defeat.

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer
    participant Screen as scr_fly_hunter_game
    participant Boss as Boss Task
    participant Bullet as Boss Bullet Task
    participant Arrow as Arrow State
    participant Buzzer as Buzzer

    Screen->>Boss: AR_GAME_BOSS_SETUP
    activate Boss
    Note right of Boss: hidden, hp = hp_max = boss_hp (starts at 10, +5 after each win)
    deactivate Boss

    Note over Screen,Boss: warning_timer reaches 0 -> game_state = GAME_STATE_BOSS
    Screen->>Boss: AR_GAME_BOSS_SPAWN
    activate Boss
    Note right of Boss: visible, x = 128, y = 15clears Fly / Butterfly / Arrow, refills arrows
    Boss->>Bullet: AR_GAME_BOSS_BULLET_SETUP + schedule first AR_GAME_BOSS_BULLET_FIRE
    Boss->>Buzzer: BUZZER_SOUND_3BEEP
    deactivate Boss

    Timer->>Screen: AR_GAME_TIME_TICK
    Screen->>Boss: AR_GAME_BOSS_RUN
    activate Boss
    alt state == BOSS_STATE_DYING
        Note right of Boss: advance explode_frame every 4 tickson finish: hide, +300 score, +1 life (capped),next_boss_score += 500, boss_hp += 5,respawn Fly + Butterfly
    else game_state == GAME_STATE_BOSS
        Note right of Boss: move left 1px + oscillate y every 2 ticksdecay shake/hit-flash timersdisplay_hp eases toward hp
        alt boss.x reaches the Border
            Note right of Boss: hide boss, reset Bulletlife-- (BUZZER_SOUND_HIT)if life == 0: post AR_GAME_RESET (once)else: game_state = NORMAL, respawn Fly + Butterfly
        end
    end
    deactivate Boss

    Screen->>Boss: AR_GAME_BOSS_DETONATOR
    activate Boss
    Boss-->>Arrow: check visible arrows vs Boss (only while BOSS_STATE_ALIVE)
    alt arrow hits Boss
        Note right of Boss: hp--, hit_flash = 6, shake_timer = 8num_arrow++ (if below max)
        alt hp > 0
            Boss->>Buzzer: BUZZER_SOUND_BANG
        else hp == 0
            Note right of Boss: state = BOSS_STATE_DYINGexplode_timer = 24
            Boss->>Bullet: AR_GAME_BOSS_BULLET_RESET
            Boss->>Buzzer: BUZZER_SOUND_BOSS_DIE
        end
    end
    deactivate Boss

    Screen->>Boss: AR_GAME_BOSS_RESET
    activate Boss
    Note right of Boss: hide, reset hp/animation, x = 128
    Boss->>Bullet: AR_GAME_BOSS_BULLET_RESET
    deactivate Boss
```

## VII. Boss Bullet Object Sequence

Boss Bullet is only active while `game_state == GAME_STATE_BOSS`. It fires itself on a self-rescheduling one-shot timer whose interval shortens as the Boss's HP drops (more aggressive at low HP).

```mermaid
sequenceDiagram
    autonumber
    participant Boss as Boss Task
    participant Timer as AR_GAME_BOSS_BULLET_FIRE Timer
    participant Screen as scr_fly_hunter_game
    participant Bullet as Boss Bullet Task
    participant Hunter as Hunter State
    participant Buzzer as Buzzer

    Boss->>Bullet: AR_GAME_BOSS_BULLET_SETUP
    activate Bullet
    Note right of Bullet: clear all bullet slots
    deactivate Bullet
    Boss->>Timer: timer_set(AR_GAME_BOSS_BULLET_FIRE, 700ms, ONE_SHOT)

    Timer->>Bullet: AR_GAME_BOSS_BULLET_FIRE
    activate Bullet
    Note right of Bullet: fire one bullet toward Hunter's yvx = -3, vy in {-1,0,1}
    Bullet->>Timer: re-schedule FIREinterval shrinks as boss.hp drops (700/500/350 ms)
    deactivate Bullet

    Screen->>Bullet: AR_GAME_BOSS_BULLET_RUN (every game tick)
    activate Bullet
    Note right of Bullet: x += vx, y += vyhide if off-screen
    Bullet-->>Hunter: check collision with Hunter
    alt bullet hits Hunter
        Note right of Bullet: hide bulletlife-- (BUZZER_SOUND_BANG)
        alt life == 0
            Note right of Bullet: post AR_GAME_RESET (once)
        end
    end
    deactivate Bullet

    Boss->>Bullet: AR_GAME_BOSS_BULLET_RESET
    activate Bullet
    Note right of Bullet: cancel FIRE timer, hide all bullets
    deactivate Bullet
```

## VIII. Bang Object Sequence

Bang is the explosion effect. It becomes visible when Fly detects a collision, and restores the matching Fly slot once its animation finishes.

```mermaid
sequenceDiagram
    autonumber
    participant Fly as Fly Task
    participant Timer as Game Tick Timer
    participant Screen as scr_fly_hunter_game
    participant Bang as Bang Task

    Screen->>Bang: AR_GAME_BANG_SETUP
    activate Bang
    Note right of Bang: hidden, action_image = frame 1
    deactivate Bang

    Fly->>Bang: collision creates a visible bang at hit position

    Timer->>Screen: AR_GAME_TIME_TICK
    Screen->>Bang: AR_GAME_BANG_UPDATE
    activate Bang
    alt bang is visible
        Note right of Bang: action_image++ every tick
    end
    alt action_image reaches AR_GAME_BANG_ACTION_IMAGE_END
        Note right of Bang: hide bang, reset frame
        Bang->>Fly: fly[i].visible = WHITE (restore slot)
    end
    deactivate Bang

    Screen->>Bang: AR_GAME_BANG_RESET
    activate Bang
    Note right of Bang: hide all bang effects
    deactivate Bang
```

## IX. Border Object Sequence

Border owns the "kill line" and drives the Fly-related level-up and game-over checks. (The Boss and Boss Bullet have their own, separate life-loss paths — see section I.)

```mermaid
sequenceDiagram
    autonumber
    participant Timer as Game Tick Timer
    participant Screen as scr_fly_hunter_game
    participant Border as Border Task
    participant Fly as Fly State
    participant Buzzer as Buzzer

    Screen->>Border: AR_GAME_BORDER_SETUP
    activate Border
    Note right of Border: x = AXIS_X_BORDER, player_life = PLAYER_MAX_LIFE (3)
    deactivate Border

    Timer->>Screen: AR_GAME_TIME_TICK
    Screen->>Border: AR_GAME_LEVEL_UP
    activate Border
    alt score >= ar_game_next_level_score
        Note right of Border: fly_speed++ (capped at MAX)next_level_score += 300
    end
    deactivate Border

    Screen->>Border: AR_GAME_CHECK_GAME_OVER
    activate Border
    Border-->>Fly: find a visible Fly at/past AXIS_X_BORDER - 3
    alt a Fly reached the Border
        alt player_life > 0
            Note right of Border: player_life--AR_GAME_SCREEN_FLASH (border flash)
            Border->>Buzzer: BUZZER_SOUND_HIT
        end
        alt player_life == 0
            Note right of Border: post AR_GAME_RESET (once)
        else
            Note right of Border: respawn that Fly at a new lane
        end
    end
    deactivate Border

    Screen->>Border: AR_GAME_BORDER_RESET
    activate Border
    Note right of Border: visible = BLACK
    deactivate Border
```

## X. Code References

| Object | Source file | Header file |
| --- | --- | --- |
| Hunter | `application/sources/app/game/fly_hunter_game/ar_game_fly_hunter.cpp` | `ar_game_fly_hunter.h` |
| Arrow | `application/sources/app/game/fly_hunter_game/ar_game_arrow.cpp` | `ar_game_arrow.h` |
| Fly | `application/sources/app/game/fly_hunter_game/ar_game_fly.cpp` | `ar_game_fly.h` |
| Butterfly | `application/sources/app/game/fly_hunter_game/ar_game_butterfly.cpp` | `ar_game_butterfly.h` |
| Boss | `application/sources/app/game/fly_hunter_game/ar_game_boss.cpp` | `ar_game_boss.h` |
| Boss Bullet | `application/sources/app/game/fly_hunter_game/ar_game_boss_bullet.cpp` | `ar_game_boss_bullet.h` |
| Bang | `application/sources/app/game/fly_hunter_game/ar_game_bang.cpp` | `ar_game_bang.h` |
| Border | `application/sources/app/game/fly_hunter_game/ar_game_border.cpp` | `ar_game_border.h` |
| Gameplay screen (orchestrates all of the above) | `application/sources/app/screens/scr_fly_hunter_game.cpp` | `scr_fly_hunter_game.h` |
