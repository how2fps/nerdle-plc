#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_logic.h"

void print(GameState *this)
{
       if (this == NULL)
       {
              return;
       }
       printf("--- Nerdle Game State ---\n");
       printf("Target: %s\n", this->answer);
       printf("Attempts Left: %d\n", this->attemptsLeft);
       printf("-------------------------\n");
}

GameState *create_game(char *target_equation)
{
       GameState *g = malloc(sizeof(GameState));
       if (g == NULL)
       {
              return NULL;
       }
       g->answer = strdup(target_equation);
       g->attemptsLeft = 6;
       print(g);
       return g;
}