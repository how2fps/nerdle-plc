#ifndef UI_H
#define UI_H

#include "game_logic.h"

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

void enter_game_view(void);
void leave_game_view(void);
void print_guess_board(const GameFSM *game);
void print_turn_status(const GameFSM *game);
void prompt_return_to_menu(void);
void print_game_lost_result(const GameFSM *game);

#endif
