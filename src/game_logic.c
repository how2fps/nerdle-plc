#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_logic.h"
#include "parser.h"

void print(GameFSM *this)
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

GameFSM *create_game(char *target_equation)
{
       GameFSM *g = malloc(sizeof(GameFSM));
       if (g == NULL)
       {
              return NULL;
       }
       g->answer = malloc(strlen(target_equation) + 1);
       if (g->answer != NULL)
       {
              strcpy(g->answer, target_equation);
       }
       g->print = print;
       g->attemptsLeft = 6;
       g->current_state = START;
       g->x = 0;
       g->y = 0;
       print(g);
       return g;

}


void transition_gamestate(GameFSM *fsm, GameEvent event)
{
       if (fsm == NULL)
       {
              return;
       }

       switch (fsm->current_state)
       {
       case START:
              if (event == GAME_EVENT_INIT)
              {
                     fsm->x = 0;
                     fsm->y = 0;
                     fsm->current_state = INPUT;
              }
              break;
       case INPUT:
              if (event == GAME_EVENT_SUBMIT_GUESS)
              {
                     fsm->current_state = VALIDATION;
              }
              break;
       case VALIDATION:
              if (event == GAME_EVENT_VALIDATION_OK)
              {
                     fsm->current_state = EVALUATION;
              }
              else if (event == GAME_EVENT_VALIDATION_FAIL)
              {
                     fsm->current_state = INPUT;
              }
              break;
       case EVALUATION:
              if (event == GAME_EVENT_EVALUATION_DONE)
              {
                     fsm->current_state = RESULT;
              }
              break;
       case RESULT:
              break;
       }
}

void validate_userinput(GameFSM *fsm, char *guess)
{
       if (fsm == NULL)
       {
              return;
       }

       if (fsm->current_state == START)
       {
              transition_gamestate(fsm, GAME_EVENT_INIT);
       }

       if (fsm->current_state == INPUT)
       {
              transition_gamestate(fsm, GAME_EVENT_SUBMIT_GUESS);
       }

       if (guess == NULL)
       {
              fsm->x = 0;
              transition_gamestate(fsm, GAME_EVENT_VALIDATION_FAIL);
              return;
       }

       if (strlen(guess) != EQUATION_LEN)
       {
              printf("Invalid equation! Must be %d characters.\n", EQUATION_LEN);
              fsm->x = 0;
              transition_gamestate(fsm, GAME_EVENT_VALIDATION_FAIL);
              return;
       }

       if (validate_equation(guess) == 0)
       {
              printf("Invalid equation! Try again.\n");
              fsm->x = 0;
              transition_gamestate(fsm, GAME_EVENT_VALIDATION_FAIL);
              return;
       }

       fsm->x = 1;
       transition_gamestate(fsm, GAME_EVENT_VALIDATION_OK);
}


/*
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
*/
