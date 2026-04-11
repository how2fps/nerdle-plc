#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#define EQUATION_LEN 8
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

typedef enum
{
        GAME_STATE_START,
        GAME_STATE_INPUT,
        GAME_STATE_VALIDATION,
        GAME_STATE_EVALUATION,
        GAME_STATE_RESULT,
        GAME_STATE_WON,
        GAME_STATE_LOST
} GameState;

typedef enum
{
        GAME_EVENT_INIT,
        GAME_EVENT_SUBMIT_GUESS,
        GAME_EVENT_VALIDATION_OK,
        GAME_EVENT_VALIDATION_FAIL,
        GAME_EVENT_EVALUATION_DONE,
        GAME_EVENT_INCORRECT_GUESS,
        GAME_EVENT_CORRECT_GUESS,
        GAME_EVENT_OUT_OF_GUESSES
} GameEvent;

typedef enum
{
        GUESS_ERROR,
        GUESS_INVALID,
        GUESS_INCORRECT,
        GUESS_CORRECT
} GuessStatus;

typedef enum
{
        VALIDATION_NULL_INPUT,
        VALIDATION_WRONG_LENGTH,
        VALIDATION_BAD_EQUATION,
        VALIDATION_REPEAT_EQUATION,
        VALIDATION_OK
} ValidationStatus;

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

typedef struct LeGameFSM{
        GameState current_state;
        char *answer;
        int max_guesses;
        int guesses_used;
        char **guess_history;
        SlotState **feedback_history;
        int has_won;
        void (*print)(struct LeGameFSM *);
} GameFSM;

void print(GameFSM *);
GameFSM *create_game(char *target_equation, int guesses, int initial_has_won);
void destroy_game(GameFSM *game);
int get_guesses_left(const GameFSM *game);
int is_game_won(const GameFSM *game);
void transition_gamestate(GameFSM *game, GameEvent event);
ValidationStatus validate_guess(GameFSM *game, const char *guess);
SlotState *evaluate_guess(GameFSM *game, const char *guess);
void game_result(GameFSM *game, const char *guess, const SlotState *feedback);
void print_guess_board(const GameFSM *game);
void print_turn_status(GameFSM *game);
GuessStatus play_guess_turn(GameFSM *game, const char *guess);
const char *game_state_to_string(GameState state);

#endif