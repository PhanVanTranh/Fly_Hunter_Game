# Design Sequence - Object

This document describes the responsibilities, states, and interactions of every object in the **Fly Hunter Game**. Each object is implemented independently and communicates through the AK Framework message system.

---

# I. Hunter

The Hunter is the player's character.

## Responsibilities

- Move vertically.
- Shoot arrows.
- Detect collisions with enemies.
- Lose lives when an enemy reaches the left border.

## States

```
IDLE
MOVE_UP
MOVE_DOWN
SHOOT
```

## Receives

```
SCREEN_ENTRY
AR_GAME_TIME_TICK
KEY_UP
KEY_DOWN
KEY_OK
```

## Sends

```
ARROW_FIRE
```

---

# II. Arrow

The Arrow is generated whenever the player shoots.

## Responsibilities

- Move from left to right.
- Detect collision with enemies.
- Destroy itself after hitting an object or leaving the screen.

## States

```
INACTIVE
ACTIVE
DESTROY
```

## Receives

```
ARROW_FIRE
AR_GAME_TIME_TICK
```

## Sends

```
FLY_HIT
BUTTERFLY_HIT
BOSS_HIT
```

---

# III. Fly

Fly is the primary enemy.

## Responsibilities

- Spawn periodically.
- Move toward the Hunter.
- Increase score when destroyed.

## States

```
HIDDEN
ACTIVE
DEAD
```

## Receives

```
FLY_SETUP
AR_GAME_TIME_TICK
```

## Sends

```
PLAYER_DAMAGE
ADD_SCORE(+10)
```

---

# IV. Butterfly

Butterfly is a special object that should **NOT** be shot.

## Responsibilities

- Spawn occasionally.
- Move toward the player.
- Reduce score if destroyed.

## States

```
HIDDEN
ACTIVE
DEAD
```

## Receives

```
BUTTERFLY_SETUP
AR_GAME_TIME_TICK
```

## Sends

```
SUB_SCORE(-20)
```

---

# V. Boss

Boss appears every fixed score milestone.

Current implementation

```
500 Points
1000 Points
1500 Points
2000 Points
...
```

## Responsibilities

- Move toward the Hunter.
- Shoot boss bullets.
- Receive multiple hits before dying.
- Reward player when defeated.

## States

```
HIDDEN
WARNING
ACTIVE
ATTACK
DEAD
```

## Receives

```
BOSS_SETUP
AR_GAME_TIME_TICK
BOSS_HIT
```

## Sends

```
BOSS_FIRE
ADD_SCORE(+100)
```

---

# VI. Boss Bullet

Boss Bullet is fired by the Boss.

## Responsibilities

- Travel toward the player.
- Damage the player on collision.

## States

```
INACTIVE
ACTIVE
DESTROY
```

## Receives

```
BOSS_FIRE
AR_GAME_TIME_TICK
```

## Sends

```
PLAYER_DAMAGE
```

---

# VII. Explosion

Explosion is a temporary visual effect.

## Responsibilities

- Display destruction animation.
- Automatically disappear.

## States

```
ACTIVE
FINISHED
```

## Receives

```
OBJECT_DESTROY
AR_GAME_TIME_TICK
```

---

# VIII. Heart

Heart represents the player's remaining lives.

## Responsibilities

- Display current HP.
- Update after damage.

## States

```
3 Hearts
2 Hearts
1 Heart
0 Heart
```

## Receives

```
PLAYER_DAMAGE
GAME_RESET
```

---

# IX. Score

Score records the player's performance.

## Responsibilities

- Increase after destroying Fly.
- Decrease after shooting Butterfly.
- Increase after defeating Boss.
- Display current score.

## Receives

```
ADD_SCORE
SUB_SCORE
GAME_RESET
```

---

# X. Border

Border defines the playable area.

## Responsibilities

- Detect enemies leaving the screen.
- Trigger player damage.
- Remove objects outside the screen.

## Receives

```
AR_GAME_TIME_TICK
```

## Sends

```
PLAYER_DAMAGE
OBJECT_REMOVE
```

---

# XI. Object Interaction

The following diagram summarizes the interactions between gameplay objects.

```text
Hunter
 │
 ├─────────────► Arrow
 │                  │
 │                  ├────────► Fly
 │                  │              │
 │                  │              └──► +10 Score
 │                  │
 │                  ├────────► Butterfly
 │                  │              │
 │                  │              └──► -20 Score
 │                  │
 │                  └────────► Boss
 │                                 │
 │                                 ├──► Boss Bullet
 │                                 │
 │                                 └──► +100 Score
 │
 └────────────► Heart (Life System)

Fly/Boss/Boss Bullet
        │
        ▼
Border
        │
        ▼
Player Damage
```

---

# XII. Object Life Cycle

```
Setup

↓

Hidden

↓

Spawn

↓

Active

↓

Collision

↓

Explosion

↓

Remove

↓

Respawn
```

---

# XIII. Summary

| Object | Purpose |
|----------|---------|
| Hunter | Player character |
| Arrow | Player projectile |
| Fly | Main enemy |
| Butterfly | Penalty object |
| Boss | High-health enemy |
| Boss Bullet | Boss projectile |
| Explosion | Hit effect |
| Heart | Player life |
| Score | Score system |
| Border | Game boundary |
