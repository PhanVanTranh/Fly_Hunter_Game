# Coding Rule

> This document defines the coding conventions and project architecture used in the **Fly Hunter Game**. Following these rules keeps the source code consistent, modular, and easy to maintain.

---

# 1. Project Structure

The project is organized into independent modules.

```
Application/
├── app_fly_hunter/
│   ├── fly_hunter_game.c
│   ├── fly_hunter_game.h
│   ├── fly.c
│   ├── fly.h
│   ├── butterfly.c
│   ├── butterfly.h
│   ├── boss.c
│   ├── boss.h
│   ├── hunter.c
│   ├── hunter.h
│   ├── arrow.c
│   ├── arrow.h
│   ├── explosion.c
│   ├── explosion.h
│   ├── border.c
│   ├── border.h
│   ├── bitmap.c
│   └── bitmap.h
```

Each module is responsible for only **one game object**.

---

# 2. Module Design Rule

Every game object follows the same structure.

Example:

```
fly.c
fly.h
```

```
boss.c
boss.h
```

```
hunter.c
hunter.h
```

Each module contains

- Object data
- Initialization
- Update logic
- Drawing routine
- Collision detection
- Reset function

This design improves readability and simplifies debugging.

---

# 3. Naming Convention

## Variables

Use lowercase with underscore.

```c
player_score
boss_health
spawn_timer
current_wave
```

---

## Constants

Use uppercase.

```c
MAX_FLY
MAX_BOSS_BULLET
BOSS_APPEAR_SCORE
PLAYER_MAX_HEALTH
```

---

## Functions

Function names follow:

```
<object>_<action>()
```

Example

```c
fly_init()
fly_update()
fly_draw()

boss_init()
boss_update()
boss_fire()

hunter_move()
hunter_draw()

arrow_update()
arrow_reset()
```

---

## Boolean Variables

Use prefixes

```
is_
has_
can_
```

Example

```c
is_alive
has_spawned
can_fire
```

---

# 4. Global Variable Rule

Avoid unnecessary global variables.

Only game-wide data should be global.

Example

```c
score

game_state

high_score
```

Object-specific variables should remain inside their own modules.

Example

```c
boss_health

boss_fire_timer

fly_speed
```

---

# 5. Header File Rule

Each module exposes only necessary interfaces.

Example

```c
void fly_init(void);
void fly_update(void);
void fly_draw(void);
```

Do not expose internal variables unless required.

---

# 6. Function Length

Each function should perform only one task.

Good

```c
boss_update()

boss_draw()

boss_fire()

boss_reset()
```

Avoid writing one large function containing every behavior.

---

# 7. Game Update Order

Every game tick updates objects in a fixed order.

```
Player

↓

Arrow

↓

Fly

↓

Butterfly

↓

Boss

↓

Boss Bullet

↓

Explosion

↓

Collision Check

↓

HUD

↓

Display Refresh
```

Keeping the update order fixed ensures deterministic gameplay.

---

# 8. Collision Detection Rule

Each object is responsible for checking collisions involving itself.

Example

```
Arrow
    ↓
Check collision with Fly

Arrow
    ↓
Check collision with Butterfly

Arrow
    ↓
Check collision with Boss
```

Avoid placing every collision check inside one huge function.

---

# 9. Bitmap Management

All bitmap resources are stored separately.

```
bitmap.c

bitmap.h
```

Each sprite has its own bitmap array.

Example

```c
hunter_bitmap

fly_bitmap

butterfly_bitmap

boss_bitmap

heart_bitmap

explosion_bitmap
```

Gameplay logic should never contain bitmap data directly.

---

# 10. State Machine

The game is implemented using a finite state machine.

```
MENU

↓

PLAY

↓

GAME OVER

↓

MENU
```

Each screen manages its own behavior independently.

---

# 11. Score Rule

Only specific events modify the score.

| Event | Score |
|--------|------:|
| Fly destroyed | +10 |
| Butterfly destroyed | -10 |
| Boss defeated | +100 |

The score should never be modified outside gameplay logic.

---

# 12. Boss Rule

The boss appears periodically based on the player's score.

```
500 points

↓

1000 points

↓

1500 points

↓

...
```

Only one boss may exist at a time.

When the boss is defeated

- Boss disappears
- Explosion animation plays
- Player receives bonus points
- Boss spawn counter advances to the next threshold

---

# 13. Memory Management

The project does **not** use dynamic memory allocation.

Avoid using

```c
malloc()

calloc()

free()
```

All game objects use fixed-size arrays.

Example

```c
Fly fly[MAX_FLY];

BossBullet bullet[MAX_BOSS_BULLET];
```

This approach is suitable for embedded systems with limited RAM.

---

# 14. Comment Style

Use comments to explain **why**, not **what**.

Good

```c
// Spawn the boss every 600 score to increase difficulty.
```

Avoid

```c
// Increase score by 10.
score += 10;
```

The code itself is already clear.

---

# 15. Coding Style

Use consistent indentation.

```c
if(condition)
{
    update();
}
else
{
    reset();
}
```

Keep one statement per line.

Avoid deeply nested conditions whenever possible.

---

# 16. Embedded Development Principles

The project follows common embedded software practices:

- Modular programming
- Event-driven architecture
- Timer-based game loop
- Fixed memory allocation
- Low RAM usage
- High readability
- Easy maintenance
- Reusable object modules

These principles make the project easier to expand with additional enemies, power-ups, levels, or gameplay mechanics in the future.
