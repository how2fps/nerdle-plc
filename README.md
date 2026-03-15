# Nerdle-PLC

A C-based clone of the popular math game **Nerdle**, developed for the SUTD 50.051 Programming Language Concepts course.

## 🕹️ Game Rules

The goal is to guess the hidden mathematical equation in 6 tries.

- **The Equation:** Must be 8 characters long and contain exactly one `=` sign.
- **The Content:** Numbers (0-9) and basic operators (`+`, `-`, `*`, `/`).
- **Mathematical Validity:** The equation must be mathematically correct according to the order of operations (PEMDAS/BODMAS).
     - _Example:_ `3+2*5=13` is valid, but `3+2*5=25` is not.
- **Leading Zeros:** Usually, numbers cannot start with a zero (e.g., `1+02=3` is invalid) unless the number is just `0`.

### Color Feedback

- 🟩 **Green**: The character is in the correct spot.
- 🟪 **Purple**: The character is in the equation but in a different spot.
- ⬛ **Black**: The character is not in the equation at all.

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
