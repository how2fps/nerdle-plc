# Nerdle-PLC

A C-based clone of the popular math game **Nerdle**, developed for the SUTD 50.051 Programming Language Concepts course.

## 🕹️ Game Rules

The goal is to guess the hidden mathematical equation in 6 tries.

- **The Equation:** Must be 8 characters long and contain exactly one `=` sign *after* all operations.
     - _Example:_ `12+12=24` is valid, but `3*8=4*6`is not.
- **The Content:** Numbers (0-9) and basic operators (`+`, `-`, `*`, `/`).
- **Mathematical Validity:** The equation must be mathematically correct according to the order of operations (PEMDAS/BODMAS).
     - _Example:_ `3+2*5=13` is valid, but `3+2*5=25` is not.
- **Leading Zeros:** Usually, numbers cannot start with a zero (e.g., `1+02=3` is invalid) unless the number is just `0`.

### Color Feedback

- 🟩 **Green**: The character is in the correct spot.
- 🟨 **Yellow**: The character is in the equation but in a different spot.
- 🟥 **Red**: The character is not in the equation at all.

---

## 🎮 Game Menu

1. Play Game
     - Play basic nerdle game with a random equation from equations.txt
2. Check Leaderboard
     - Check recent leaderboard statistics
3. Add New Equation
     - Add a custom equation to equations.txt
4. Challenge Mode
     - Enter a custom nerdle, then challenge a friend locally
5. Watch Replay
     - Rewatch past games played saved as binary files
6. Exit
     - Exit the game interface

---

## 🛠️ Build Instructions

This project is configured to be compiled using `gcc` and `make`.

### Prerequisites

- **Compiler**: GCC (via MSYS2 UCRT64 recommended on Windows)
- **Build Tool**: Make

### Compilation

To compile the game, run the following command in your terminal:

```bash
make
```

### Execution

After compiling, run the following command in your terminal:

```bash
nerdle
```

Have fun!
