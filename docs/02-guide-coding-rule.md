# Coding Rule

> This document defines the coding conventions and project architecture actually used in the **Fly Hunter Game** source code. Following these rules keeps the codebase consistent, modular, and easy to maintain.

---

## 1. Architecture: Message-Driven, Not OOP

Fly Hunter is **not** built around per-object `init()/update()/draw()` classes. Every screen and every game object is a single **message handler function** that reacts to signals posted by the AK framework's task/timer/message system:

```cpp
void ar_game_fly_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case AR_GAME_FLY_SETUP:      /* ... */ break;
    case AR_GAME_FLY_RUN:        /* ... */ break;
    case AR_GAME_FLY_DETONATOR:  /* ... */ break;
    case AR_GAME_FLY_RESET:      /* ... */ break;
    default: break;
    }
}
```

There is no `fly_init()`, `fly_update()`, or `fly_draw()` API — everything happens inside the `switch` on `msg->sig`, and objects are advanced by the screen posting them a signal every game tick, not by being polled.

## 2. Real Project Structure

```
application/sources/app/
├── game/fly_hunter_game/
│   ├── ar_game_fly_hunter.cpp / .h   # Hunter
│   ├── ar_game_arrow.cpp / .h        # Arrow
│   ├── ar_game_fly.cpp / .h          # Fly
│   ├── ar_game_butterfly.cpp / .h    # Butterfly
│   ├── ar_game_boss.cpp / .h         # Boss
│   ├── ar_game_boss_bullet.cpp / .h  # Boss Bullet
│   ├── ar_game_bang.cpp / .h         # Explosion effect
│   └── ar_game_border.cpp / .h       # Border / life & level-up checks
└── screens/
    ├── scr_startup.cpp / .h
    ├── scr_menu_game.cpp / .h
    ├── scr_game_setting.cpp / .h
    ├── scr_fly_hunter_game.cpp / .h  # orchestrates every object above
    ├── scr_game_over.cpp / .h
    ├── scr_charts_game.cpp / .h
    ├── scr_idle.cpp / .h
    └── screens_bitmap.cpp / .h       # all bitmap byte arrays
```

Each object module is responsible for **one game object only**, and is driven entirely by the screen that owns it (`scr_fly_hunter_game.cpp`).

## 3. Naming Convention

| Prefix | Meaning | Example |
| --- | --- | --- |
| `scr_` | Screen module (dynamic view + handler) | `scr_fly_hunter_game.cpp` |
| `ar_game_` | Game object module | `ar_game_border.cpp`, `ar_game_butterfly.cpp` |
| `AR_GAME_` | Game-level constants / message signals | `AR_GAME_CHECK_GAME_OVER` |
| `AC_DISPLAY_` | Display/system-level signals | `AC_DISPLAY_BUTTON_MODE_PRESSED` |

**Variables:** lowercase with underscores (`player_life`, `ar_game_score`, `warning_timer`).  
**Constants:** uppercase with underscores (`PLAYER_MAX_LIFE`, `AR_GAME_LEVEL_UP_SCORE`).  
**Message signals:** `AR_GAME_<OBJECT>_<ACTION>` (`AR_GAME_FLY_HUNTER_SETUP`, `AR_GAME_BOSS_BULLET_FIRE`). There is **no** `FH_GAME_*` prefix anywhere in the codebase — any doc or diagram using that prefix is inaccurate and should be corrected.

## 4. Screen Module Structure

Every screen implements exactly 3 parts:

```cpp
// 1. Dynamic view struct - links a screen to its render function
view_dynamic_t dyn_view_item_xxx = { {ITEM_TYPE_DYNAMIC}, view_scr_xxx };

// 2. Screen struct - registered with the FSM
view_screen_t scr_xxx = { &dyn_view_item_xxx, ITEM_NULL, ITEM_NULL, .focus_item = 0 };

// 3. Message handler - reacts to SCREEN_ENTRY / button / timer signals
void scr_xxx_handle(ak_msg_t* msg) { switch (msg->sig) { ... } }
```

**Rule:** the view function (`view_scr_xxx`) that draws to the LCD **must always end with `view_render.update();`**. Forgetting this call is a real bug pattern in this codebase — the content is drawn into the buffer but never pushed to the physical screen, so it silently "does nothing" until some unrelated event happens to call `update()` afterwards.

**Rule:** whenever a screen needs to render immediately after a state change (button press, `SCREEN_ENTRY`, etc.), call its view function explicitly. Do not assume the framework auto-renders on screen transition.

**Rule:** when transitioning to a follow-up screen (e.g. Game Over) from inside another screen's handler, call `SCREEN_TRAN` directly at that point rather than delaying it with an intermediate timer-driven state. `scr_fly_hunter_game.cpp` transitions to `scr_game_over` immediately inside its `AR_GAME_RESET` handler for this reason — there is deliberately no intermediate "result" screen.

## 5. Object Module Structure

Every game object (Fly, Butterfly, Boss, Arrow, ...) is handled purely through messages — objects never call each other's functions directly. Communication always goes through:

```cpp
task_post_pure_msg(AR_GAME_FLY_ID, AR_GAME_FLY_RUN);
```

A typical object lifecycle:

```
AR_GAME_<OBJECT>_SETUP    // called once on SCREEN_ENTRY
AR_GAME_<OBJECT>_RUN      // called every AR_GAME_TIME_TICK
AR_GAME_<OBJECT>_DETONATOR (or similar) // collision checks
AR_GAME_<OBJECT>_RESET    // called on AR_GAME_RESET
```

**Rule:** never post the same reset/transition message twice for a single event. Duplicate `task_post_pure_msg` calls for the same signal (e.g. `AR_GAME_RESET`) can cause a handler to run twice, leading to double score-saving or double screen transitions. `ar_game_border.cpp` posts `AR_GAME_RESET` **exactly once** when `player_life` reaches 0 — keep any new life-loss path (Boss, Boss Bullet, or future objects) consistent with that rule.

**Rule:** be aware that **more than one object can independently cost the player a life** in this game — a Fly reaching the Border, the Boss reaching the Border, and a Boss Bullet hitting the Hunter are three separate code paths. Each must guard against firing `AR_GAME_RESET` more than once, and must not assume it is the only source of life loss.

## 6. Timers

- Use `TIMER_PERIODIC` for continuously repeating game ticks (`AR_GAME_TIME_TICK`).
- Use `TIMER_ONE_SHOT` for one-time delayed events (e.g. showing the idle screen, or the Boss Bullet's self-rescheduling `AR_GAME_BOSS_BULLET_FIRE` timer).
- Always call `timer_remove_attr()` for a timer's signal when leaving the state that owns it, to avoid stale timers firing after a screen transition.

## 7. Bitmap Management

All bitmap resources are declared in one place:

```
application/sources/app/screens/screens_bitmap.h / .cpp
```

Gameplay object files (`ar_game_*.cpp`) only *reference* bitmap symbols (`bitmap_fly_I`, `bitmap_butterfly_I`, `bitmap_heart`, ...) when drawing — the raw byte arrays never live inside gameplay logic files.

## 8. Memory Management

The project does **not** use dynamic memory allocation. Avoid `malloc()` / `calloc()` / `free()`. All game objects use fixed-size arrays sized by constants such as:

```cpp
#define NUM_FLYs           (3)
#define MAX_NUM_ARROW       AR_GAME_SETTING_NUM_ARROW_MAX
#define NUM_BOSS_BULLET     (...)
```

This is standard practice for embedded systems with limited RAM (16 KB on this board).

## 9. Score Rule

Only specific, well-defined events modify `ar_game_score`:

| Event | Score |
| --- | ---: |
| Fly destroyed | **+10** |
| Butterfly destroyed | **-20** |
| Boss defeated | **+300** |

## 10. Comments

- Use `APP_DBG_SIG("...")` at the top of every handled message case for traceability during debugging.
- Keep large ASCII-art section headers (`/**** ... ****/`) only for major view-drawing blocks, to keep files easy to scan.
- Prefer explaining **why**, not **what** — the message-driven `switch` structure already makes "what happens on this signal" self-evident.

## 11. Embedded Development Principles

- Message-driven, not polled — objects react to signals, they are not ticked via direct function calls.
- Fixed-size, statically allocated object arrays.
- Screens own their objects; objects never reach into other objects' state directly.
- Persistent data (settings, scores) always goes through the EEPROM read/write helpers in `app_eeprom.cpp`, never raw driver calls from gameplay code.

These principles keep the codebase easy to extend with new enemies, power-ups, or screens without destabilizing existing behavior.
