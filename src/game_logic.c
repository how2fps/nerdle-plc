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
       g->answer = malloc(strlen(target_equation) + 1);
       if (g->answer != NULL)
       {
              strcpy(g->answer, target_equation);
       }
       g->attemptsLeft = 6;
       print(g);
       return g;
}

static SlotInput next_slot_input(
       char guess_char, 
       char answer_char, 
       char *answer_str)
{
       return (SlotInput){
              .in_answer = (strchr(answer_str, guess_char) != NULL) ? 1 : 0,
              .correct_position = (guess_char == answer_char) ? 1 : 0
       };
}

static SlotState next_slot_state(SlotState current, SlotInput input)
{
       if (input.in_answer && input.correct_position)
       {
              return CORRECT;
       }
       else if (input.in_answer)
       {
              return PARTIAL;
       }
       else
       {
              return WRONG;
       }
}

SlotState *evaluate_guess(const char *guess, const char *answer)
{
       size_t i;
       SlotInput input;
       SlotState *result = malloc(sizeof(SlotState) * strlen(answer));
       if (result == NULL || guess == NULL || answer == NULL || strlen(guess) != strlen(answer))
       {
              return NULL;
       }
       for (i = 0; i < strlen(answer); i++)
       {
              input = next_slot_input(guess[i], answer[i], (char *)answer);
              result[i] = next_slot_state(WRONG, input);
       }
       return result;
}