# Runtime Signal Processing

This document describes the runtime message flow of the Fly Hunter Game.
The gameplay is driven by the AK Framework scheduler and message-based event
system. Each game object operates as an independent task and communicates
through runtime messages.

---

# 1. Game Start

Figure 1 illustrates the initialization sequence after the player selects
**Start Game** from the Main Menu.

During this stage, the screen initializes all gameplay objects including
Hunter, Arrow, Fly, Butterfly, Boss, Boss Bullet and Border before starting
the periodic game timer.

<p align="center">
<img src="images/design-sequence/fh_game_start_sequence.png" width="100%">
</p>

<p align="center">
<b>Figure 1:</b> Game start sequence logic
</p>

---

# 2. Game Playing

Figure 2 illustrates the runtime execution of the gameplay loop.

Every game frame is triggered by **AR_GAME_TIME_TICK**.
Each object updates its internal state, performs collision detection,
updates the score, checks player life, and finally renders the next frame
to the LCD.

<p align="center">
<img src="images/design-sequence/fh_game_play_sequence.png" width="100%">
</p>

<p align="center">
<b>Figure 2:</b> Gameplay runtime sequence
</p>

---

# 3. Game Reset

Figure 3 illustrates the reset procedure executed after the player loses all
remaining lives.

The game stops the periodic timer, clears all runtime objects,
stores the high score if necessary, resets internal variables and returns
to the Main Menu.

<p align="center">
<img src="images/design-sequence/fh_game_reset_sequence.png" width="100%">
</p>

<p align="center">
<b>Figure 3:</b> Game reset sequence
</p>
