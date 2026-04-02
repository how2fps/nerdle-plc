#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#define EQUATION_LEN 8

typedef enum
{
        START,
        INPUT,
        VALIDATION,
        EVALUATION,
        RESULT

} game_state;

typedef enum
{
        GAME_EVENT_INIT,
        GAME_EVENT_SUBMIT_GUESS,
        GAME_EVENT_VALIDATION_OK,
        GAME_EVENT_VALIDATION_FAIL,
        GAME_EVENT_EVALUATION_DONE
} GameEvent;

typedef struct LeGameFSM{
        char *answer;
        void (*print)(struct LeGameFSM *);
        int attemptsLeft;
        game_state current_state;
        int x; 
        int y;
} GameFSM;

void print(GameFSM *);

GameFSM *create_game(char *target_equation);


void transition_gamestate(GameFSM *fsm, GameEvent event);
void validate_userinput(GameFSM *fsm, char *guess);


#endif