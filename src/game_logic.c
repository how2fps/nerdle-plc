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
       int i;
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
       g->print = print;
       g->max_guesses = max_guesses;
       g->guesses_used = 0;
       g->guess_history = malloc(sizeof(char *) * max_guesses);
       g->feedback_history = malloc(sizeof(SlotState *) * max_guesses);

       if (g->guess_history == NULL || g->feedback_history == NULL)
       {
              free(g->feedback_history);
              free(g->guess_history);
              free(g->answer);
              free(g);
              return NULL;
       }
       for (i = 0; i < max_guesses; i++)
       {
              g->guess_history[i] = malloc(sizeof(char) * (EQUATION_LEN + 1));
              g->feedback_history[i] = malloc(sizeof(SlotState) * EQUATION_LEN);
              if (g->guess_history[i] == NULL || g->feedback_history[i] == NULL)
              {
                     while (i >= 0)
                     {
                            free(g->guess_history[i]);
                            free(g->feedback_history[i]);
                            i--;
                     }
                     free(g->feedback_history);
                     free(g->guess_history);
                     free(g->answer);
                     free(g);
                     return NULL;
              }
              memset(g->guess_history[i], '\0', EQUATION_LEN + 1);
       }
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
       int i;
       if (game == NULL)
       {
              return;
       }

       if (game->guess_history != NULL && game->feedback_history != NULL)
       {
              for (i = 0; i < game->max_guesses; i++)
              {
                     free(game->guess_history[i]);
                     free(game->feedback_history[i]);
              }
       }

       free(game->feedback_history);
       free(game->guess_history);

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
       int i;
       
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

       for(i = 0; i < game->guesses_used; i++){
              if(!strcmp(guess, game->guess_history[i])){
                     transition_gamestate(game, GAME_EVENT_VALIDATION_FAIL);
                     return VALIDATION_REPEAT_EQUATION;
              }
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
       input.in_answer = (strchr(answer_str, guess_char) != NULL) ? 1 : 0;
       input.correct_position = (guess_char == answer_char) ? 1 : 0;
       return input;
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

SlotState *evaluate_guess(GameFSM *game, const char *guess)
{
       size_t i;
       SlotInput input;
       SlotState *result = malloc(sizeof(SlotState) * strlen(game->answer));
       if (result == NULL || guess == NULL || game->answer == NULL || strlen(guess) != strlen(game->answer))
       {
              return NULL;
       }
       for (i = 0; i < strlen(game->answer); i++)
       {
              input = next_slot_input(guess[i], game->answer[i], (char *)game->answer);
              result[i] = next_slot_state(WRONG, input);
              
       }
       return result;
}

void game_result(GameFSM *game, const char *guess, const SlotState *feedback)
{
       int i;
       int all_correct = 1;
       int row;

       if (game == NULL || guess == NULL || feedback == NULL)
       {
              return;
       }

       row = game->guesses_used;
       if (row < game->max_guesses)
       {
              memcpy(game->guess_history[row], guess, EQUATION_LEN);
              game->guess_history[row][EQUATION_LEN] = '\0';
              memcpy(game->feedback_history[row], feedback, sizeof(SlotState) * EQUATION_LEN);
       }

       for (i = 0; i < EQUATION_LEN; i++)
       {
              if (feedback[i] != CORRECT)
              {
                     all_correct = 0;
              }
       }

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

void print_guess_board(const GameFSM *game)
{
       int row;
       int col;

       if (game == NULL)
       {
              return;
       }

       printf("\n");
       for (row = 0; row < game->max_guesses; row++)
       {
              if (row < game->guesses_used)
              {
                     for (col = 0; col < EQUATION_LEN; col++)
                     {
                            if (game->feedback_history[row][col] == CORRECT)
                            {
                                   printf("[" GREEN "%c" RESET "]", game->guess_history[row][col]);
                            }
                            else if (game->feedback_history[row][col] == PARTIAL)
                            {
                                   printf("[" YELLOW "%c" RESET "]", game->guess_history[row][col]);
                            }
                            else
                            {
                                   printf("[" RED "%c" RESET "]", game->guess_history[row][col]);
                            }
                     }
              }
              else
              {
                     for (col = 0; col < EQUATION_LEN; col++)
                     {
                            printf("[_]");
                     }
              }
              printf("\n");
       }
       printf("\n");
}

void print_turn_status(GameFSM *game)
{
       print_guess_board(game);
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
              else if (validation_status == VALIDATION_REPEAT_EQUATION)
              {
                     printf("Equation already guessed!\n\n");
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
       print_turn_status(game);

       if (is_game_won(game) == 1)
       {
              return GUESS_CORRECT;
       }

       return GUESS_INCORRECT;
}

