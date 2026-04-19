#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evaluator.h"
#include "game_logic.h"
#include "parser.h"
#include "game_ui.h"

/*
 * Debug helper that prints a brief summary of the current game state
 * (number of guesses remaining) to stdout.
 */
void print(GameFSM *this)
{
       if (this == NULL)
       {
              return;
       }
       printf("--- Nerdle Game State ---\n");
       printf("Guesses Left: %d\n", get_guesses_left(this));
       printf("-------------------------\n");
}

/*
 * Allocates and fully initialises a new GameFSM on the heap.
 * Sets up the answer string, guess/feedback history arrays (one row
 * per allowed guess), the character frequency table used by the
 * two-pass feedback algorithm, and the initial FSM state.
 *
 * All allocations are checked; on any failure the function frees
 * everything allocated so far and returns NULL (no memory leaks).
 *
 * Returns a heap-allocated GameFSM, or NULL on allocation failure.
 * The caller must eventually call destroy_game() to free the memory.
 */
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
       return g;
}

/*
 * Advances the FSM to its next state based on the current state and
 * the incoming event. Invalid event/state combinations are silently
 * ignored (the FSM stays in its current state).
 */
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
              {
                     fsm->current_state = GAME_STATE_INPUT;
              }
              break;

       case GAME_STATE_INPUT:
              if (event == GAME_EVENT_SUBMIT_GUESS)
              {
                     fsm->current_state = GAME_STATE_VALIDATION;
              }
              break;

       case GAME_STATE_VALIDATION:
              if (event == GAME_EVENT_VALIDATION_OK)
              {
                     fsm->current_state = GAME_STATE_EVALUATION;
              }
              else if (event == GAME_EVENT_VALIDATION_FAIL)
              {
                     /* invalid guess — return to input without consuming a turn */
                     fsm->current_state = GAME_STATE_INPUT;
              }
              break;

       case GAME_STATE_EVALUATION:
              if (event == GAME_EVENT_EVALUATION_DONE)
              {
                     fsm->current_state = GAME_STATE_RESULT;
              }
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
              /* terminal states; only GAME_EVENT_INIT can restart them */
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

/*
 * Checks that a guess string is acceptable before it is evaluated.
 * Validation fails (and the FSM transitions back to GAME_STATE_INPUT)
 * if any of the following are true:
 *   - guess or game is NULL
 *   - length is not exactly EQUATION_LEN
 *   - the equation is syntactically invalid (FSM in parser.c)
 *   - the equation causes division by zero
 *   - the two sides of the equation are not numerically equal
 *   - the guess has already been submitted in this session
 *
 * On success, transitions the FSM to GAME_STATE_EVALUATION.
 */
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
       /* check for invalid syntax */
       if (validate_equation(guess) == 0)
       {
              transition_gamestate(game, GAME_EVENT_VALIDATION_FAIL);
              return VALIDATION_BAD_EQUATION;
       }
       /* check for division by 0*/
       if (process_line((char *)guess, 0) == -1)
       {
              transition_gamestate(game, GAME_EVENT_VALIDATION_FAIL);
              return VALIDATION_BAD_EQUATION;
       }
       /* check for invalid equation */
       if (process_line((char *)guess, 0) == 0)
       {
              transition_gamestate(game, GAME_EVENT_VALIDATION_FAIL);
              return VALIDATION_BAD_EQUATION;
       }

       for (i = 0; i < game->guesses_used; i++)
       {
              if (!strcmp(guess, game->guess_history[i]))
              {
                     transition_gamestate(game, GAME_EVENT_VALIDATION_FAIL);
                     return VALIDATION_REPEAT_EQUATION;
              }
       }

       transition_gamestate(game, GAME_EVENT_VALIDATION_OK);
       return VALIDATION_OK;
}

/*
 * Builds a SlotInput struct for a single character position by comparing
 * the guessed character against the answer character at the same position.
 */
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

/*
 * Determines the feedback colour for a single slot using the two-pass algorithm.
 *
 * Pass 1 (iteration == 1): handles exact matches only.
 *   - CORRECT  if the character is in the right position; decrements its
 *              frequency count so it cannot also trigger a PARTIAL on pass 2.
 *   - WRONG    placeholder for all non-exact-match slots (overwritten in pass 2).
 *
 * Pass 2 (iteration == 2): handles non-exact-match slots.
 *   - CORRECT  if it was already marked correct in pass 1 (left unchanged).
 *   - PARTIAL  if the character appears somewhere in the answer AND its
 *              remaining frequency count is > 0; decrements the count.
 *   - WRONG    otherwise.
 *
 * The frequency table (game->freq) is pre-populated by evaluate_guess()
 * before either pass runs.
 */
static SlotState next_slot_state(int iteration, GameFSM *game, SlotInput input)
{
       if (iteration == 1)
       {
              if (input.correct_position)
              {
                     game->freq[(unsigned char)input.character]--;
                     return CORRECT;
              }
              else
              {
                     return WRONG;
              }
       }
       else if (iteration == 2)
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

/*
 * Generates a per-slot feedback array for the given guess against the
 * stored answer using the two-pass algorithm.
 *
 * Before the passes begin, game->freq is populated with the frequency
 * of each character in the answer. Pass 1 then resolves CORRECT slots
 * and decrements their counts; pass 2 resolves PARTIAL and WRONG slots
 * using the remaining counts.
 *
 * Returns a heap-allocated SlotState array of length EQUATION_LEN,
 * or NULL on allocation failure or mismatched lengths.
 */
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
       /* build the character frequency table from the answer */
       memset(game->freq, 0, sizeof(game->freq));
       for (i = 0; i < len; i++)
       {
              game->freq[(unsigned char)game->answer[i]]++;
       }
       /* run two passes over every slot to assign CORRECT, PARTIAL, or WRONG */
       for (j = 1; j <= 2; j++)
       {
              for (i = 0; i < len; i++)
              {
                     input = next_slot_input(guess[i], game->answer[i], (char *)game->answer);
                     result[i] = next_slot_state(j, game, input);
              }
       }
       return result;
}

/*
 * Records a completed guess and its feedback into the history arrays,
 * increments guesses_used, and transitions the FSM to the appropriate
 * next state:
 *   - GAME_STATE_WON   if all slots are CORRECT
 *   - GAME_STATE_LOST  if no guesses remain
 *   - GAME_STATE_INPUT otherwise (game continues)
 */
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
              {                                                             /* GAME LOST */
                     transition_gamestate(game, GAME_EVENT_OUT_OF_GUESSES); /* to GAME_STATE_LOST */
              }
              else
              {
                     transition_gamestate(game, GAME_EVENT_INCORRECT_GUESS); /* to GAME_STATE_INPUT */
              }
       }
}

/*
 * The main entry point for a single guess attempt. Drives the GameFSM
 * through the full validate → evaluate → record cycle for one guess:
 *
 *   1. Initialise the FSM if it is still in GAME_STATE_START.
 *   2. Advance from GAME_STATE_INPUT to GAME_STATE_VALIDATION.
 *   3. Call validate_guess(); print an error and return GUESS_INVALID
 *      if validation fails (the guess does not consume a turn).
 *   4. Call evaluate_guess() to produce per-slot feedback.
 *   5. Advance to GAME_STATE_RESULT and call game_result() to record
 *      the guess and update the FSM state.
 *   6. Return GUESS_CORRECT if the game is now won, GUESS_INCORRECT
 *      otherwise.
 *
 * Returns:
 *   GUESS_CORRECT   - the guess matched the answer (game won)
 *   GUESS_INCORRECT - valid guess, but not the answer
 *   GUESS_INVALID   - guess failed validation (turn not consumed)
 *   GUESS_ERROR     - unexpected state or NULL pointer
 */
GuessStatus play_guess_turn(GameFSM *game, const char *guess)
{
       SlotState *feedback;
       ValidationStatus validation_status;

       if (game == NULL || guess == NULL)
       {
              return GUESS_ERROR;
       }

       /* advance through START -> INPUT -> VALIDATION */
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
                     printf(COLOR_RED "Invalid equation! Must be %d characters.\n\n" COLOR_RESET, EQUATION_LEN);
              }
              else if (validation_status == VALIDATION_BAD_EQUATION)
              {
                     printf(COLOR_RED "Invalid equation! Try again.\n\n" COLOR_RESET);
              }
              else if (validation_status == VALIDATION_REPEAT_EQUATION)
              {
                     printf(COLOR_RED "Equation already guessed!\n\n" COLOR_RESET);
              }
              else
              {
                     printf(COLOR_RED "Invalid input. Try again.\n\n" COLOR_RESET);
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

       if (is_game_won(game) == 1)
       {
              return GUESS_CORRECT;
       }

       return GUESS_INCORRECT;
}

/*
 * Frees all heap memory owned by the GameFSM, including each row of
 * the guess and feedback history arrays, the history pointer arrays
 * themselves, the answer string, and the GameFSM struct.
 */
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

/*
 * Returns the number of guesses the player still has available.
 * Clamps to 0 if guesses_used somehow exceeds max_guesses.
 * Returns 0 if the GameFSM pointer is NULL.
 */
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

/*
 * Returns 1 if the player has correctly guessed the equation, 0 otherwise.
 * Returns 0 if the GameFSM pointer is NULL.
 */
int is_game_won(const GameFSM *game)
{
       if (game == NULL)
       {
              return 0;
       }

       return game->has_won;
}
