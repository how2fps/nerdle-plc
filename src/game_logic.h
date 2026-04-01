#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

typedef struct LeGameState{
        char *answer;
        void (*print)(struct LeGameState *);
        int attemptsLeft;

} GameState;

void print(GameState *);

GameState *create_game(char *target_equation);

typedef enum
{
        WRONG,
        PARTIAL,
        CORRECT
} SlotState;

typedef struct{
        int in_answer;
        int correct_position;
} SlotInput;

SlotState *evaluate_guess(const char *guess, const char *answer);

#endif