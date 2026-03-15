#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

typedef struct LeGameState{
        char *answer;
        void (*print)(struct LeGameState *);
        int attemptsLeft;

} GameState;

void print(GameState *);

GameState *create_game(char *target_equation);

#endif