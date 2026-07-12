# Getting Started

This guide explains how to build, flash, and run **Fly Hunter** on the **AK Embedded Base Kit (STM32L151)**. It also introduces the real project structure and the basic development workflow.

---

## 1. Project Overview

Fly Hunter is a 2D shooting mini-game built on top of the **AK framework** — a lightweight, message-driven embedded application framework (finite-state screens + task/timer/message objects).

The player controls a Hunter standing on the left side of the screen and shoots incoming Flies before they reach the Border. Different objects provide different rewards or penalties, and a Boss periodically raises the difficulty.

Main features:

- Message-driven game objects (Hunter, Arrow, Fly, Butterfly, Boss, Boss Bullet, Bang, Border)
- Boss battle system with an escalating HP/reward loop
- Score and Top-3 High Score, persisted in EEPROM
- Adjustable arrow count and Fly speed, plus a sound on/off toggle
- Buzzer melodies for game events
- Idle-screen timeout with return-to-previous-screen support

---

## 2. Hardware Requirements

- AK Embedded Base Kit (STM32L151CBT6)
- ST-Link programmer (for flashing over SWD)
- USB Type-C cable
- PC running Windows or Linux

---

## 3. Software Requirements

<!-- TODO: confirm the exact toolchain/IDE version you use to build this project -->

- ARM GCC toolchain (the project's `Makefile`/`Makefile.mk` files are GCC-based, not a CubeIDE project)
- ST-Link driver / STM32CubeProgrammer, for flashing
- GNU Make
- Git

---

## 4. Clone Repository

```bash
git clone https://github.com/PhanVanTranh/Fly_Hunter_Game.git
cd Fly_Hunter_Game
```

---

## 5. Real Project Structure

```
Fly_Hunter_Game/
├── application/
│   ├── Makefile
│   └── sources/
│       ├── app/
│       │   ├── game/
│       │   │   └── fly_hunter_game/     # Hunter, Arrow, Fly, Butterfly,
│       │   │                            # Boss, Boss Bullet, Bang, Border
│       │   ├── screens/                 # scr_startup, scr_menu_game,
│       │   │                            # scr_game_setting, scr_fly_hunter_game,
│       │   │                            # scr_game_over, scr_charts_game, scr_idle
│       │   ├── app.cpp / app.h          # top-level signal enum (AR_GAME_*)
│       │   ├── app_eeprom.cpp / .h      # EEPROM read/write for settings & score
│       │   └── task_list.cpp / .h       # task IDs and routing table
│       ├── ak/                          # AK framework core (fsm, message, task, timer)
│       ├── common/                      # view_render — the LCD draw wrapper
│       └── driver/                      # button, buzzer, eeprom, led, OLED drivers
├── boot/                                # Bootloader sources (separate Makefile)
├── docs/                                # Documentation (this folder)
├── resources/                           # Images, prebuilt binaries, board photos
└── README.md
```

> Note: this is **not** a `Core/ Drivers/ Middlewares/` STM32CubeMX-style layout — the AK framework uses its own `application/ + boot/` split, with the application further divided into `app/` (game + screens) and `driver/` (hardware wrappers).

---

## 6. Build the Project

```bash
cd application
make
```

<!-- TODO: document any project-specific build flags/targets (e.g. `make HW_VERSION=v3`) -->

The `boot/` bootloader is built the same way, from its own `Makefile`, and is normally flashed once and rarely touched afterward.

---

## 7. Flash the Firmware

Pre-built binaries are already available under `resources/bin/` for reference:
- Bootloader image for the AK Base Kit
- Firmware images matching different hardware revisions

<!-- TODO: describe your actual flashing procedure (ST-Link Utility / STM32CubeProgrammer command line / UART bootloader) -->

---

## 8. Running the Game

After flashing:

1. The Main Menu appears (Start Game / Settings / High Score / Exit).
2. Select **Start Game**.
3. Control the Hunter using **UP / DOWN**, shoot with **MODE**.
4. Shoot Flies before they reach the Border, and avoid the Butterfly.
5. Survive Boss encounters to keep climbing the score.

---

## 9. Game Controls

| Button | Function |
|---|---|
| **UP** | Move Hunter up / Navigate up in menus |
| **DOWN** | Move Hunter down / Navigate down in menus |
| **MODE** | Shoot an arrow / Confirm & adjust a setting value |

---

## 10. Gameplay Rules (Summary)

### Fly
- Main target — shoot it for **+10 score**.
- Reaching the Border costs the player a life.

### Butterfly
- A decoy — shooting it costs **-20 score**.
- Flying past the Border does **not** cost a life.

### Boss
- Appears every **500 points** (500, 1000, 1500, ...).
- Fires bullets at the Hunter and requires multiple arrow hits to defeat.
- Defeating it rewards **+300 score** and **+1 extra life** (capped at max), and makes the next Boss tougher (+5 HP).

Full per-object detail: [03-design-sequence-object.md](./03-design-sequence-object.md).

---

## 11. Game Over

The game ends when the player's lives reach **0**. The session score is compared against the Top-3 high scores stored in EEPROM and saved if it qualifies. See [06-eeprom-data-storage.md](./06-eeprom-data-storage.md).

---

## 12. High Score

The **High Score** screen (reached from the Main Menu, or from Game Over via **UP**) displays the top 3 scores stored in EEPROM.

---

## 13. Settings

The **Settings** screen lets players configure:

- **ARROWS** — number of arrows available per life (`num_arrow`)
- **FLY SPEED** — Fly movement speed (`fly_speed`)
- **SOUND** — on/off toggle (`silent`)

All three are persisted to EEPROM. See [06-eeprom-data-storage.md](./06-eeprom-data-storage.md) for the exact struct layout.

---

## 14. Development Workflow

```
Modify source code
        ↓
make (in application/)
        ↓
Flash the resulting firmware image
        ↓
Run and test on the physical board
        ↓
Debug & iterate
```

---

## 15. Further Reading

| Document | Description |
|---|---|
| [02-guide-coding-rule.md](./02-guide-coding-rule.md) | Coding conventions and the message-driven object pattern |
| [03-design-sequence-object.md](./03-design-sequence-object.md) | Per-object sequence diagrams |
| [04-design-sequence-runtime.md](./04-design-sequence-runtime.md) | Full runtime signal flow |
| [05-buzzer-audio.md](./05-buzzer-audio.md) | Sound effects reference |
| [06-eeprom-data-storage.md](./06-eeprom-data-storage.md) | EEPROM layout for settings & scores |

---

## 16. Troubleshooting

### Build failed
- Check the ARM GCC toolchain is installed and on `PATH`.
- Check your `make` version.
- Build from inside `application/` (not the repo root).

### Cannot flash
- Check the ST-Link driver and USB cable.
- Confirm the board is in the correct boot mode.
- Confirm STM32CubeProgrammer (or your flashing tool) detects the ST-Link.

### Display is blank
- Confirm the firmware actually flashed successfully (check the programmer's output).
- Power-cycle / reset the board after flashing.
- If the screen was previously showing Game Over and now looks frozen, see the note on `view_render.update()` in [02-guide-coding-rule.md](./02-guide-coding-rule.md) — a missing `update()` call is a common cause of "nothing renders" bugs in this codebase.
