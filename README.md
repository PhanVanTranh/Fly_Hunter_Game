# Fly Hunter Game

<div align="center">
  <video src="https://github.com/user-attachments/assets/e24911bf-5073-4d07-a8e8-738927585d1b" controls width="480"></video>

![Profile Views](https://komarev.com/ghpvc/?username=PhanVanTranh&color=007ec6&style=flat-square)

</div>                            

---
## Documentation                            

| File | Description |
|---|---|
| [README.md](./README.md) | Project overview (this file) |
| [docs/01-guide-getting-started.md](./docs/01-guide-getting-started.md) | How to build, flash and run the project |
| [docs/02-guide-coding-rules.md](./docs/02-guide-coding-rules.md) | Coding conventions used in this project |
| [docs/03-design-sequence-object.md](./docs/03-design-sequence-object.md) | Sequence diagrams for each game object |
| [docs/04-design-sequence-runtime.md](./docs/04-design-sequence-runtime.md) | Runtime signal/message flow |


## Introduction                            

**Fly Hunter** is a 2D shooting mini-game built on the AK Embedded Base Kit, inspired by classic side-scrolling shooters. The player controls a hunter standing at a fixed border on the left side of the screen, aiming and shooting arrows at incoming Flies before they cross the border, while avoiding a decoy **Butterfly** that punishes careless shots. Every few hundred points, a **Boss** appears with its own HP bar and bullet attacks, raising the difficulty and testing the player's reflexes.

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

The following section describes the gameplay and core mechanics of Fly Hunter. It serves as a reference for understanding the game's mechanics and firmware implementation.

<table align="center">
  <tr>
    <td align="center"><img src="resources/images/screens/scr_menu_game.png" alt="Game menu" width="480"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 3:</em></strong> Menu screen</p>

<table align="center">
  <tr>
    <td align="center"><img src="resources/images/screens/scr_setting.png" alt="Setting" width="480"/></td>
  </tr>
</table>
<p align="center"><strong><em>Figure 4:</em></strong> Setting screen</p>

When the game starts, the Main Menu is displayed with the following options:

- **Start Game:** Begin a new game.
- **Settings:** Configure gameplay parameters such as sound and difficulty.
- **High Score:** Display the top three highest scores saved in memory.
- **Exit:** Return to the idle screen.


<p align="center"> <img src="resources/images/screens/scr_game_fly_hunter.png" width="320"><br> <em><b>Figure 5.</b> Gameplay Screen</em> </p>

During gameplay, the player controls a hunter positioned on the left side of the screen. The objective is to shoot incoming Flies while avoiding the Butterfly decoy and surviving Boss encounters.

#### Objects in the Game:

| Object | Bitmap | Description |
|---|---|---|
| **Hunter** | <p align="center"><img src="resources/images/bitmap/archery_1.png" width="100"/></p>| The player character. Stands at the shooting position and fires arrows.|
| **Arrow** | <p align="center"><img src="resources/images/bitmap/arrow.png" width="100"/></p>| Projectile fired by the Hunter. Travels across the screen and destroys any Fly, Butterfly, or Boss it hits. |
| **Fly** | <p align="center"><img src="resources/images/bitmap/bitmap_fly_hunter_I.png" width="100"/></p> | The main enemy wave. Flies in from the right side toward the border on the left. If it crosses the border, the player loses one life. Spawn speed increases as the score rises. |
| **Butterfly** | <p align="center"><img src="resources/images/bitmap/bitmap_butterfly_I.png" width="100"/></p> | A decoy object. Shows a floating `-20` text and penalizes the score when involved, adding risk/reward to the fight. |
| **Boss** | <p align="center"><img src="resources/images/bitmap/bitmap_fly_hunter_II.png" width="100"/></p> | A large enemy that spawns once the score passes a threshold. |
| **Boss Bullet** | <p align="center"><img src="resources/images/bitmap/bitmap_bullet.png" width="100"/></p> | Projectile fired back by the Boss at the player. |
| **Bang (Explosion)** | <p align="center"><img src="resources/images/bitmap/bang_1.png" width="100"/></p>| Generic explosion effect played whenever an object is destroyed. |
| **Border** | <p align="center"><img src="resources/images/bitmap/bitmap_border.png" width="100"/></p> | The "kill line" on the left edge of the play area. Enemies crossing it cost the player a life; the border flashes white when the player is hit. |
| **Heart** |<p align="center"><img src="resources/images/bitmap/bitmap_heart.png" width="100"/></p> | Displayed at the bottom-left of the HUD to represent the player's remaining lives. |

### III - How to Play

**Controls:**  
- **MODE** — Shoot an arrow (in-game) / Confirm & adjust a value (in the Settings menu).
- **UP** — Move the Hunter up / Navigate up in menus.
- **DOWN** — Move the Hunter down / Navigate down in menus.

**Game Mechanics:**  
- Destroy Flies and the Boss with arrows to increase your **Score** (+10 per Fly, +300 per Boss).
- Shooting a **Butterfly** costs **-20 score** — treat it as a decoy to avoid, not a target.
- Every Fly that crosses the **Border** costs the player **1 life** out of **3** (see the Heart icons in the HUD). The Boss reaching the Border, or a Boss Bullet hitting the Hunter, also costs a life.
- The game ends (**Game Over**) when all lives reach **0**.
- Difficulty increases progressively: every 300 points, the Fly spawn speed increases.
- A Boss spawns once the score reaches **500**, and again every **+500** points after that. When the threshold approaches, a **"WARNING - BOSS INCOMING"** banner flashes with a 3-beep alert before the Boss spawns.
- Defeating a Boss rewards **+300 score** and **+1 extra life** (capped at 3), and increases the Boss's Health Points (HP) for the next encounter.
- Before starting, players can fine-tune the challenge from the **Settings** screen: number of arrows, Fly speed, and silent mode (mute sound effects).

<table align="center">
  <tr>
    <td align="center"><img src="resources/images/screens/scr_game_over_2_4x.png" alt="fly humter game over screen 1" width="480"/></td>
  </tr>
</table>

### IV - Basic Game Sequence Logic

The screens and objects communicate purely through the message/timer system of the AK framework (`SCREEN_ENTRY`, `AR_GAME_TIME_TICK`, `AR_GAME_CHECK_GAME_OVER`, `AR_GAME_RESET`, ...).

> **Note:** for the full, per-object breakdown of this flow, see [docs/03-design-sequence-object.md](./docs/03-design-sequence-object.md) and [docs/04-design-sequence-runtime.md](./docs/04-design-sequence-runtime.md).

```mermaid
%%{init: {
'theme':'base',
'themeVariables':{
'primaryColor':'#90CAF9',
'primaryTextColor':'#000',
'primaryBorderColor':'#1976D2',
'lineColor':'#333',
'actorBorder':'#1976D2',
'actorBkg':'#81C784',
'activationBorderColor':'#ffffff',
'activationBkgColor':'#4FC3F7'
}}}%%

sequenceDiagram
    actor Player
    participant AK as AK Framework
    participant Screen as scr_fly_hunter_game
    participant Hunter
    participant Arrow
    participant Fly
    participant Butterfly
    participant Boss
    participant BossBullet as Boss Bullet
    participant Border

    Player->>Screen: SCREEN_ENTRY (Start Game)
    activate Screen

    Screen->>Hunter: AR_GAME_FLY_HUNTER_SETUP
    Screen->>Arrow: AR_GAME_ARROW_SETUP
    Screen->>Fly: AR_GAME_FLY_SETUP
    Screen->>Butterfly: AR_GAME_BUTTERFLY_SETUP
    Screen->>Boss: AR_GAME_BOSS_SETUP
    Screen->>BossBullet: AR_GAME_BOSS_BULLET_SETUP
    Screen->>Border: AR_GAME_BORDER_SETUP

    Note over Screen: ar_game_state = GAME_PLAY next_boss_score = 500
    Screen->>Screen: timer_set(AR_GAME_TIME_TICK, PERIODIC)

    loop Every AR_GAME_TIME_TICK

        AK->>Hunter: AR_GAME_FLY_HUNTER_UPDATE
        Hunter->>Hunter: apply UP/DOWN movement

        AK->>Arrow: AR_GAME_ARROW_RUN
        Arrow->>Arrow: move active arrows right

        AK->>Fly: AR_GAME_FLY_RUN
        Fly->>Fly: move left, animate

        AK->>Butterfly: AR_GAME_BUTTERFLY_RUN
        Butterfly->>Butterfly: move on sine-wave path

        alt Arrow hits Fly
            Arrow-->>Fly: AR_GAME_FLY_DETONATOR
            Fly-->>Screen: +10 Score, refill 1 arrow
        end

        alt Arrow hits Butterfly
            Arrow-->>Butterfly: AR_GAME_BUTTERFLY_DETONATOR
            Butterfly-->>Screen: -20 Score (shown as floating text)
        end

        Screen->>Border: AR_GAME_LEVEL_UP
        Note right of Border: score >= next_level_score (300, 600, ...)-> Fly speed++

        alt Score reaches 500, 1000, 1500, ...
            Screen->>Screen: GAME_STATE_WARNING flash "WARNING BOSS INCOMING" + 3-beep
            Screen->>Boss: AR_GAME_BOSS_SPAWN
        end

        alt Boss Active
            AK->>Boss: AR_GAME_BOSS_RUN
            Boss->>BossBullet: fire toward Hunter's row
            BossBullet-->>Hunter: collision check -> life-- if hit
        end

        alt Arrow hits Boss
            Arrow-->>Boss: AR_GAME_BOSS_DETONATOR
            Boss-->>Boss: hp--
        end

        alt Boss HP == 0
            Boss-->>Screen: +300 Score, +1 life (capped at 3)
        end

        Screen->>Border: AR_GAME_CHECK_GAME_OVER
        alt a Fly (or the Boss) reaches the Border
            Border-->>Screen: life--
        end

        alt life == 0
            Screen->>AK: AR_GAME_RESET (posted once)
            Screen->>Player: SCREEN_TRAN -> scr_game_over (GAME OVER)
        end

    end

    deactivate Screen
```


## Contact & Support

[![GitHub](https://img.shields.io/badge/GitHub-PhanVanTranh-181717?style=flat&logo=github)](https://github.com/PhanVanTranh)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-0A66C2?style=flat&logo=linkedin)]([www.linkedin.com/in/tranh-phan-3b7785311](https://www.linkedin.com/in/tranh-phan-3b7785311))
[![Gmail](https://img.shields.io/badge/Gmail-Contact-EA4335?style=flat&logo=gmail&logoColor=white)](mailto:phantranh2304@gmail.com)
