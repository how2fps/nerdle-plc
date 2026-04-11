#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_logic.h"
#include "parser.h"

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

void print(GameFSM *this)
{
       if (this == NULL)
       {
              return;
       }
       printf("--- Nerdle Game State ---\n");
       printf("Target: %s\n", this->answer);
       printf("Guesses Left: %d\n", get_guesses_left(this));
       printf("-------------------------\n");
}

GameFSM *create_game(char *target_equation, int max_guesses, int initial_has_won)
{
       GameFSM *g = malloc(sizeof(GameFSM));
       if (g == NULL)
       {
              return NULL;
       }
       g->answer = malloc(strlen(target_equation) + 1);
       if (g->answer == NULL)
       {
              free(g);
              return NULL;
       }

       strcpy(g->answer, target_equation);
       g->current_guess = malloc(sizeof(SlotState) * strlen(target_equation));
       if (g->current_guess == NULL)
       {
              free(g->answer);
              free(g);
              return NULL;
       }
       g->current_guess_str = NULL;
       memset(g->freq, 0, sizeof(g->freq));
       g->print = print;
       g->max_guesses = max_guesses;
       g->guesses_used = 0;
       if (initial_has_won == 1)
       {
              g->current_state = GAME_STATE_WON;
       }
       else
       {
              g->current_state = GAME_STATE_START;
       }
       g->has_won = initial_has_won;
       print(g);
       return g;

}

void destroy_game(GameFSM *game)
{
       if (game == NULL)
       {
              return;
       }

       free(game->answer);
       free(game);
}

int get_guesses_left(const GameFSM *game)
{
       int guesses_left;

       if (game == NULL)
       {
              return 0;
       }

       guesses_left = game->max_guesses - game->guesses_used;
       if (guesses_left < 0)
       {
              guesses_left = 0;
       }

       return guesses_left;
}

int is_game_won(const GameFSM *game)
{
       if (game == NULL)
       {
              return 0;
       }

       return game->has_won;
}

const char *game_state_to_string(GameState state)
{
       switch (state)
       {
       case GAME_STATE_START:
              return "START";
       case GAME_STATE_INPUT:
              return "INPUT";
       case GAME_STATE_VALIDATION:
              return "VALIDATION";
       case GAME_STATE_EVALUATION:
              return "EVALUATION";
       case GAME_STATE_RESULT:
              return "RESULT";
       case GAME_STATE_WON:
              return "WON";
       case GAME_STATE_LOST:
              return "LOST";
       default:
              return "UNKNOWN";
       }
}


void transition_gamestate(GameFSM *fsm, GameEvent event)
{
       if (fsm == NULL)
       {
              return;
       }

       switch (fsm->current_state)
       {
       case GAME_STATE_START:
              if (event == GAME_EVENT_INIT)
                     fsm->current_state = GAME_STATE_INPUT;
              break;

       case GAME_STATE_INPUT:
              if (event == GAME_EVENT_SUBMIT_GUESS)
                     fsm->current_state = GAME_STATE_VALIDATION;
              break;

       case GAME_STATE_VALIDATION:
              if (event == GAME_EVENT_VALIDATION_OK)
                     fsm->current_state = GAME_STATE_EVALUATION;
              else if (event == GAME_EVENT_VALIDATION_FAIL)
                     fsm->current_state = GAME_STATE_INPUT;
              break;

       case GAME_STATE_EVALUATION:
              if (event == GAME_EVENT_EVALUATION_DONE)
                     fsm->current_state = GAME_STATE_RESULT;
              break;

       case GAME_STATE_RESULT:
              if (event == GAME_EVENT_INCORRECT_GUESS)
              {
                     fsm->current_state = GAME_STATE_INPUT;
              }
              else if (event == GAME_EVENT_CORRECT_GUESS)
              {
                     fsm->current_state = GAME_STATE_WON;
              }
              else if (event == GAME_EVENT_OUT_OF_GUESSES)
              {
                     fsm->current_state = GAME_STATE_LOST;
              }
              break;

       case GAME_STATE_WON:
       case GAME_STATE_LOST:
              if (event == GAME_EVENT_INIT)
              {
                     fsm->current_state = GAME_STATE_INPUT;
              }
              break;

       default:
              fsm->current_state = GAME_STATE_START;
              break;
       }
}

ValidationStatus validate_guess(GameFSM *game, const char *guess)
{
       if (game == NULL || guess == NULL)
       {
              if (game != NULL)
              {
                     transition_gamestate(game, GAME_EVENT_VALIDATION_FAIL);
              }
              return VALIDATION_NULL_INPUT;
       }

       if (strlen(guess) != EQUATION_LEN)
       {
              transition_gamestate(game, GAME_EVENT_VALIDATION_FAIL);
              return VALIDATION_WRONG_LENGTH;
       }

       if (validate_equation(guess) == 0)
       {
              transition_gamestate(game, GAME_EVENT_VALIDATION_FAIL);
              return VALIDATION_BAD_EQUATION;
       }

       transition_gamestate(game, GAME_EVENT_VALIDATION_OK);
       return VALIDATION_OK;
}

static SlotInput next_slot_input(
       char guess_char, 
       char answer_char, 
       char *answer_str)
{
       SlotInput input;
       input.character = guess_char;
       input.correct_position = (guess_char == answer_char) ? 1 : 0;
       return input;
}

static SlotState next_slot_state(int iteration, GameFSM *game, SlotInput input)
{
       if(iteration == 1)
       {
              if(input.correct_position){
                     game->freq[(unsigned char)input.character]--;
                     return CORRECT;
              }
              else
              {
                     return WRONG;
              }
       }
       else if(iteration == 2)
       {
              if (input.correct_position)
              {
                     return CORRECT;
              }
              else if (strchr(game->answer, input.character) != NULL && game->freq[(unsigned char)input.character] > 0)
              {
                     game->freq[(unsigned char)input.character]--;
                     return PARTIAL;
              }
              else
              {
                     return WRONG;
              }
       }
       else
       {
              return WRONG;
       }
}

SlotState *evaluate_guess(GameFSM *game, const char *guess)
{
       size_t i, j;
       size_t len;
       SlotInput input;
       SlotState *result = malloc(sizeof(SlotState) * strlen(game->answer));
       if (result == NULL || guess == NULL || game->answer == NULL || strlen(guess) != strlen(game->answer))
       {
              return NULL;
       }
       len = strlen(game->answer);
       if (strlen(guess) != len)
       {
              free(result);
              return NULL;
       }
       memset(game->freq, 0, sizeof(game->freq));
       for (i = 0; i < len; i++)
       {
              game->freq[(unsigned char)game->answer[i]]++;
       }
       for(j = 1; j <= 2; j++)
       {
              for (i = 0; i < len; i++)
              {
                     input = next_slot_input(guess[i], game->answer[i], (char *)game->answer);
                     result[i] = next_slot_state(j, game, input);
                     
              }
       }
       return result;
}

void game_result(GameFSM *game, const char *guess, const SlotState *feedback)
{
       int i;
       int all_correct = 1;

       if (game == NULL || guess == NULL || feedback == NULL)
       {
              return;
       }

       for (i = 0; i < EQUATION_LEN; i++)
       {
              if (feedback[i] == CORRECT)
              {
                     printf(GREEN "%c" RESET, guess[i]);
              }
              else if (feedback[i] == PARTIAL)
              {
                     printf(YELLOW "%c" RESET, guess[i]);
                     all_correct = 0;
              }
              else
              {
                     printf(RED "%c" RESET, guess[i]);
                     all_correct = 0;

              }
       }
       printf("\n");

       if (get_guesses_left(game) > 0)
       {
              game->guesses_used++;
       }

       if (all_correct)
       {      
              /* GAME WON */
              game->has_won = 1;
              transition_gamestate(game, GAME_EVENT_CORRECT_GUESS); /* to GAME_STATE_WON */
       }
       else
       {
              if (get_guesses_left(game) == 0)
              {      /* GAME LOST */
                     transition_gamestate(game, GAME_EVENT_OUT_OF_GUESSES); /* to GAME_STATE_LOST */
              }
              else
              {
                     transition_gamestate(game, GAME_EVENT_INCORRECT_GUESS); /* to GAME_STATE_INPUT */
              }
       }
}

void print_round_status(GameFSM *game)
{
       if (is_game_won(game) == 1)
       {
              printf("You got the answer!\n");
       }
       else
       {
              printf("Guesses left: %d\n\n", get_guesses_left(game));
       }
}

/* "main" function of game logic */
GuessStatus play_guess_turn(GameFSM *game, const char *guess)
{
       SlotState *feedback;
       ValidationStatus validation_status;

       if (game == NULL || guess == NULL)
       {
              return GUESS_ERROR;
       }

       if (game->current_state == GAME_STATE_START)
       {
              transition_gamestate(game, GAME_EVENT_INIT); /* to GAME_STATE_INPUT  */
       }

       if (game->current_state == GAME_STATE_INPUT)
       {
              transition_gamestate(game, GAME_EVENT_SUBMIT_GUESS); /* to GAME_STATE_VALIDATION  */
       }

       if (game->current_state != GAME_STATE_VALIDATION)
       {
              return GUESS_ERROR;
       }

       validation_status = validate_guess(game, guess);  
       /* validate_guess() transitions fsm to GAME_STATE_EVALUTAION if validation passes, 
       back to GAME_STATE_INPUT without decreasing guesses left */
       if (validation_status != VALIDATION_OK)
       {
              if (validation_status == VALIDATION_WRONG_LENGTH)
              {
                     printf("Invalid equation! Must be %d characters.\n\n", EQUATION_LEN);
              }
              else if (validation_status == VALIDATION_BAD_EQUATION)
              {
                     printf("Invalid equation! Try again.\n\n");
              }
              else
              {
                     printf("Invalid input. Try again.\n\n");
              }
              return GUESS_INVALID;
       }

       feedback = evaluate_guess(game, guess); 
       if (feedback == NULL)
       {
              return GUESS_ERROR;
       }

       transition_gamestate(game, GAME_EVENT_EVALUATION_DONE); /* to GAME_STATE_RESULT */
       game_result(game, guess, feedback); 
       /* game_result() transitions fsm to GAME_STATE_WON, GAME_STATE_LOST,
        or back to GAME_STATE_INPUT on win, lose and guesses left > 0 respectively */
       free(feedback);
       print_round_status(game);

       if (is_game_won(game) == 1)
       {
              return GUESS_CORRECT;
       }

       return GUESS_INCORRECT;
}
