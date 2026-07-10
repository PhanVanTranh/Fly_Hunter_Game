# Getting Started

This guide explains how to build, flash, and run **Fly Hunter** on the **AK Embedded Kit (STM32L151)**. It also introduces the project structure and the basic development workflow.

---

# 1. Project Overview

Fly Hunter is a simple 2D shooting game developed for the **AK Embedded Kit**.

The player controls a hunter standing on the left side of the screen and shoots incoming targets before they reach the border. Different objects provide different rewards or penalties, making the gameplay progressively more challenging.

Main features:

- Multiple enemy types
- Boss battle system
- Score and High Score
- Difficulty settings
- Sound effects
- Object animation
- Timer-driven game loop

---

# 2. Hardware Requirements

- AK Embedded Kit (STM32L151)
- ST-Link Programmer
- USB Type-C cable
- PC running Windows or Ubuntu

---

# 3. Software Requirements

Install the following tools before building the project.

## Required

- STM32CubeProgrammer
- STM32 ST-Link Driver
- ARM GCC Toolchain
- Make
- Git

Optional:

- STM32CubeIDE
- Visual Studio Code

---

# 4. Clone Repository

```bash
git clone https://github.com/PhanVanTranh/Fly_Hunter_Game.git

cd Fly_Hunter_Game
```

---

# 5. Project Structure

```
Fly_Hunter_Game
│
├── Core/
├── Drivers/
├── Middlewares/
├── Resources/
│
├── Application/
│   ├── Game/
│   ├── Objects/
│   ├── Screens/
│   └── Sound/
│
├── docs/
├── Makefile
└── README.md
```

---

# 6. Build Project

Compile the firmware.

```bash
make clean

make
```

Successful compilation generates:

```
build/Fly_Hunter.bin
```

or

```
build/Fly_Hunter.hex
```

depending on the build configuration.

---

# 7. Flash Firmware

Connect the AK Embedded Kit through ST-Link.

Using STM32CubeProgrammer:

1. Open STM32CubeProgrammer
2. Connect via ST-Link
3. Select:

```
Fly_Hunter.bin
```

4. Download
5. Reset the board

---

# 8. Running the Game

After flashing:

1. The Main Menu appears.
2. Select **START GAME**.
3. Control the hunter using the navigation buttons.
4. Shoot enemies before they reach the border.
5. Try to achieve the highest score.

---

# 9. Game Controls

| Button | Function |
|---------|----------|
| UP | Move Hunter Up |
| DOWN | Move Hunter Down |
| ENTER | Shoot / Confirm |
| LEFT | Navigate Menu |
| RIGHT | Navigate Menu |

---

# 10. Gameplay Rules

### Fly

- Main enemy
- Shoot to gain points

### Butterfly

- Friendly object
- Shooting it reduces the score

### Boss

Appears every **500 points**.

Boss can:

- Move vertically
- Shoot bullets
- Require multiple hits to defeat

Defeating the boss rewards the player with:

- Bonus score
- Extra life

---

# 11. Game Over

The game ends when:

- Player lives become zero

The final score is stored as High Score if it exceeds the previous record.

---

# 12. High Score

The High Score screen displays the best score stored in Flash memory.

Players can compare their latest score with the current record.

---

# 13. Difficulty Settings

The Settings menu allows players to configure:

- Starting difficulty
- Sound ON/OFF

Higher difficulty increases:

- Enemy movement speed
- Enemy spawn frequency
- Boss challenge

---

# 14. Development Workflow

Typical workflow:

```
Modify source code

        ↓

Compile project

        ↓

Generate firmware

        ↓

Flash STM32

        ↓

Run game

        ↓

Debug & Improve
```

---

# 15. Documentation

Detailed design documents are available in the **docs/** directory.

| Document | Description |
|----------|-------------|
| 02-guide-coding-rule.md | Coding conventions |
| 03-design-sequence-object.md | Object interaction |
| 04-design-sequence-runtime.md | Runtime signal processing |

---

# 16. Troubleshooting

### Build failed

Check:

- ARM GCC installation
- PATH environment variable
- Make version

---

### Cannot flash

Check:

- ST-Link driver
- USB cable
- Boot mode
- STM32CubeProgrammer connection

---

### Display is blank

Verify:

- LCD initialization
- Firmware downloaded successfully
- Reset after flashing

---

# 17. Next Reading

Continue with the following documents:

- **02-guide-coding-rule.md**
- **03-design-sequence-object.md**
- **04-design-sequence-runtime.md**
