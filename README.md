# 🪰 Fly Hunter Game

<p align="center">
  <img src="docs/images/logo.png" width="220">
</p>

<p align="center">

An embedded arcade shooting game developed for the <b>AK Embedded Kit (STM32)</b>.

Shoot flies, avoid bees, defeat powerful bosses, and achieve the highest score!

</p>

---

# 📖 Introduction

Fly Hunter Game is a 2D arcade shooting game designed and implemented on the AK Embedded Kit using the STM32 microcontroller.

The player controls a hunter and shoots arrows to eliminate incoming flies before they cross the defense line. Throughout the game, dangerous bees and powerful bosses appear, making the gameplay increasingly challenging.

The project demonstrates real-time graphics rendering, sprite animation, collision detection, sound effects, finite state machine (FSM), timer scheduling, EEPROM data storage, and embedded software architecture.

---

# ✨ Features

- 🪰 Shoot flies to earn points
- 🐝 Bees act as dangerous obstacles
- 👾 Boss battle every 600 points
- ❤️ Multiple life system
- 💥 Explosion animations
- 🔊 Sound effects
- 🏆 High score ranking
- 💾 EEPROM data storage
- 🎮 Interactive menu system
- ⚙️ Game settings
- 📊 High score screen
- 🚀 Startup animation

---

# 🎮 Gameplay

## Main Menu

- Start Game
- Settings
- High Score
- Exit

---

## During Gameplay

- Shoot flies to gain points.
- Prevent flies from crossing the defense line.
- Each escaped fly costs one life.
- Shooting bees decreases your score.
- Bosses appear every **600 points**.
- Defeat bosses to continue progressing.
- The game ends when all lives are lost.

---

# 👾 Enemies

## Fly

- Main target
- +10 score when defeated

---

## Bee

- Dangerous obstacle
- Shooting a bee decreases your score

---

## Boss

- Appears every 600 points
- Has multiple HP
- Shoots bullets toward the player
- Explosion animation when defeated

---

# ❤️ Player

The player can

- Move vertically
- Shoot arrows
- Destroy flies
- Fight bosses
- Protect the defense line

---

# 🏆 High Score

The game stores

- Current score
- Top 1
- Top 2
- Top 3

using EEPROM, allowing scores to persist after restarting the device.

---

# 🔊 Sound Effects

Different sounds are played for

- Startup
- Shooting
- Hit
- Boss warning
- Boss defeated
- Menu navigation
- Game Over

---

# 🛠 Hardware

- AK Embedded Kit
- STM32 MCU
- 128×64 OLED Display
- Buzzer
- Push Buttons
- EEPROM

---

# 📂 Project Structure

```
Fly_Hunter_Game
│
├── app/
├── driver/
├── middleware/
├── screen/
│     ├── Startup
│     ├── Menu
│     ├── Game
│     ├── Setting
│     ├── High Score
│     └── Game Over
│
├── game/
│     ├── Fly
│     ├── Bee
│     ├── Boss
│     ├── Arrow
│     ├── Border
│     ├── Bang
│     └── Fly Hunter
│
└── resources/
```

---

# 🧩 Software Architecture

The project is organized using a modular architecture.

- Finite State Machine (FSM)
- Screen Manager
- Timer Scheduler
- Event-driven Message System
- Object-based Game Engine

Each game object operates independently and communicates through messages.

---

# 🎯 Controls

| Button | Function |
|---------|----------|
| UP | Move Up |
| DOWN | Move Down |
| MODE | Shoot / Select |

---

# 📸 Screenshots

## Startup

<p align="center">
<img src="docs/images/startup.png" width="300">
</p>

## Main Menu

<p align="center">
<img src="docs/images/menu.png" width="300">
</p>

## Gameplay

<p align="center">
<img src="docs/images/gameplay.png" width="300">
</p>

## Boss Battle

<p align="center">
<img src="docs/images/boss.png" width="300">
</p>

## Game Over

<p align="center">
<img src="docs/images/gameover.png" width="300">
</p>

---

# 🚀 Future Improvements

- More Boss types
- Multiple stages
- Power-up system
- New enemy types
- Combo score system
- Particle effects
- Better AI
- Multiple difficulty levels

---

# 👨‍💻 Author

**Phan Van Tranh**

Embedded Systems Engineering

GitHub:

https://github.com/PhanVanTranh

---

# 📄 License

This project is developed for educational purposes.

Feel free to fork, modify, and improve it.
