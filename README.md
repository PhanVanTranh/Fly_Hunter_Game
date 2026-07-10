# Fly Hunter Game

<div align="center">
  <video src="https://github.com/user-attachments/assets/e24911bf-5073-4d07-a8e8-738927585d1b" controls width="480"></video>
</div>

---
## Documentation

| File | Description |
|---|---|
| [README.md](./README.md) | Project overview (this file) |
| [docs/01-guide-getting-started.md](./docs/01-guide-getting-started.md) | How to build, flash and run the project |
| [docs/02-guide-coding-rules.md](./docs/02-guide-coding-rules.md) | Coding conventions used in this project |
| [docs/03-design-sequence-object.md](./docs/03-design-sequence-object.md) | Sequence diagrams for each game object (Hunter, Arrow, Fly, Bee, Boss...) |
| [docs/04-design-sequence-runtime.md](./docs/04-design-sequence-runtime.md) | Runtime signal/message flow (Mermaid diagrams) |


## Introduction

**Fly Hunter** is a 2D shooting mini-game built on the AK Embedded Base Kit, inspired by classic side-scrolling shooters. The player controls a hunter standing at a fixed border on the left side of the screen, aiming and shooting arrows at incoming flies, bees, and meteoroids before they cross the border. Every few hundred points, a **Boss** appears with its own HP bar and bullet attacks, raising the difficulty and testing the player's reflexes.

Like other embedded games built on the AK platform, Fly Hunter is also a practical showcase of embedded system design concepts:
- **System design** — screens and game objects organized as independent, event-driven modules (UML-style).
- **Process management** — each game object (Hunter, Arrow, Fly, Butterfly, Boss, Boss Bullet, Bang, Border) is handled as its own task with its own message handler.
- **Communication** — objects talk to each other purely through **Signal / Timer / Message**, no direct function coupling.
- **Control logic** — screen navigation is implemented as a **Finite State Machine**, and gameplay difficulty scales through a simple state machine.

### I. Hardware

<table align="center">
  <tr>
    <td align="center"><img src="resources/images/board/ak-embedded-base-kit-version-3.jpg" alt="AK Embedded Base Kit - STM32L151 - v3.0" width="480"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 1:</em></strong> AK Embedded Base Kit - STM32L151</p>

[AK Embedded Base Kit](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu) is an evaluation kit aimed at intermediate and advanced embedded software learners.

The kit integrates a **1.54" OLED LCD**, **3 push buttons**, and **a buzzer** capable of playing short melodies, giving you everything you need to study **event-driven systems** through hands-on game-machine design.
It also exposes **RS485**, the **Qwiic Connect System**, and **Grove** connectors, so it doubles as a convenient prototyping board for real-world embedded projects.

**MCU Overview:**

```text
SoC Name : STM32L151CBT6
RAM      : 16 KB

Flash Partitions Layout
----------------------
[ 0x08000000 - 0x08001FFF ] : Bootloader Partition (8 KB)
=> AK Bootloader

[ 0x08002000 - 0x08002FFF ] : BSF Shared Partition (4 KB)
=> Used for data sharing between Bootloader and Application

[ 0x08003000 - 0x0801FFFF ] : Application Partition (116 KB)
=> Fly hunter firmware
```

**MCU Naming Convention:**

| Part | Meaning |
|---|---|
| `STM32` | STMicroelectronics 32-bit MCU family. |
| `L` | Low-power series. |
| `151` | STM32L151 product line. |
| `C` | 48-pin package. |
| `B` | 128 KB Flash memory. |
| `T` | LQFP package. |
| `6` | Industrial temperature grade. |


<table align="center">
  <tr>
    <td align="center"><img src="resources/images/board/board-view-top-bottom.png" width="900"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 2:</em></strong> Board view Top + Bottom </p>

### II - Game Description and Objects

<!-- TODO: insert screenshots/GIFs of the main menu and gameplay screen here -->

| Object | Bitmap | Description |
|---|---|---|
| **Hunter** | <p align="center"><img src="resources/images/bitmap/arcery_1.png" width="100"/></p>| The player character. Stands at the shooting position and fires arrows. Has a 2-frame idle/shoot animation. |
| **Arrow** | <p align="center"><img src="resources/images/bitmap/arrow.png" width="100"/></p>| Projectile fired by the Hunter. Travels across the screen and destroys any Fly, Bee, or Boss it hits. |
| **Fly** | <p align="center"><img src="resources/images/bitmap/bitmap_fly_hunter_I.png" width="100"/></p> | The main enemy wave. Flies in from the right side toward the border on the left. If it crosses the border, the player loses one life. Spawn speed increases as the score rises (Level Up mechanic). |
| **Butterfly** | `bitmap_butterfly_I` / `II` / `III` | A special enemy. Shows a floating `-20` text and penalizes the score when involved, adding risk/reward to the fight. |
| **Boss** | `bitmap_fly_boss` | A large enemy that spawns once the score passes a threshold (starting at 1000, increasing over time). Has its own HP bar drawn above/below it, shakes when hit, and dies through a 3-frame explosion animation (`bitmap_bang_I/II/III`). |
| **Boss Bullet** | (filled circle) | Projectile fired back by the Boss at the player. |
| **Bang (Explosion)** | `bitmap_bang_I` / `II` / `III` | Generic explosion effect played whenever an enemy (Fly/Bee/Boss) is destroyed. |
| **Border** | (vertical flashing line) | The "kill line" on the left edge of the play area. Enemies crossing it cost the player a life; the border flashes white when the player is hit. |
| **Heart** | `bitmap_heart` | Displayed at the bottom-left of the HUD to represent the player's remaining lives. |
| **Score** | text | Displayed at the top-right of the HUD, increases as enemies are destroyed. |

### III - How to Play

**Controls:**
- **MODE** — Shoot an arrow (in-game) / Confirm & adjust a value (in the Settings menu).
- **UP** — Move the Hunter up / Navigate up in menus.
- **DOWN** — Move the Hunter down / Navigate down in menus.
- **UP (long press)** — Jump setting values to their maximum.
- **DOWN (long press)** — Jump setting values to their minimum.

**Game Mechanics:**
- Destroy Flies, Bees, and the Boss with arrows to increase your **Score**.
- Every enemy that crosses the **Border** costs the player **1 life** (see the Heart icons in the HUD).
- The game ends (**Game Over**) when all lives reach **0**.
- Difficulty increases progressively: every 300 points, the Fly spawn speed increases (`AR_GAME_LEVEL_UP`).
- When the score approaches a Boss threshold, a **"WARNING - BOSS INCOMING"** banner flashes with a 3-beep alert before the Boss spawns.
- Before starting, players can fine-tune the challenge from the **Settings** screen: number of arrows, meteoroid spawn speed, and silent mode (mute sound effects).

<!-- TODO: insert a screenshot of the Game Over screen here -->

### IV - Basic Game Sequence Logic

The screens and objects communicate purely through the message/timer system of the AK framework (`SCREEN_ENTRY`, `AR_GAME_TIME_TICK`, `AR_GAME_CHECK_GAME_OVER`, `AR_GAME_RESET`, ...). A high-level flow:

```
Menu -> [SCREEN_ENTRY] scr_fly_hunter_game
      -> AR_GAME_TIME_TICK (periodic)
            -> update Hunter / Arrow / Fly / Bee / Boss / Boss Bullet
            -> AR_GAME_CHECK_GAME_OVER
                  -> life-- when an enemy crosses the Border
                  -> when life == 0 -> post AR_GAME_RESET
      -> AR_GAME_RESET
            -> save score, reset all objects
            -> SCREEN_TRAN -> scr_game_over
```

See [docs/04-design-sequence-runtime.md](./docs/04-design-sequence-runtime.md) for the full Mermaid sequence diagram.

## Contact & Support

<!-- TODO: replace with your real contact information -->
[![GitHub](https://img.shields.io/badge/GitHub-PhanVanTranh-181717?style=flat&logo=github)](https://github.com/PhanVanTranh)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-0A66C2?style=flat&logo=linkedin)](www.linkedin.com/in/tranh-phan-3b7785311)
[![Gmail](https://img.shields.io/badge/Gmail-Contact-EA4335?style=flat&logo=gmail&logoColor=white)](phantranh2304@gmail.com)
