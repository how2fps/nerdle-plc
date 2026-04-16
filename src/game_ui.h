#ifndef UI_H
#define UI_H

#include "game_logic.h"
#include "leaderboard.h"

/* -- Colours ------------------------------------------------------------ */
#define COLOR_RESET "\033[0m"
#define COLOR_BOLD "\033[1m"
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_CYAN "\033[36m"
#define COLOR_GOLD "\033[93m"
#define COLOR_SILVER "\033[37m"
#define COLOR_BRONZE "\033[33m"
#define RED COLOR_RED
#define GREEN COLOR_GREEN
#define YELLOW COLOR_YELLOW
#define RESET COLOR_RESET

/* -- Menu ------------------------------------------------------ */
void print_menu(void);
void print_section_header(const char *title);

/* -- Game board / status ------------------------------------------------ */
void enter_game_view(void);
void leave_game_view(void);
void print_guess_board(const GameFSM *game);
void print_turn_status(const GameFSM *game);
void print_game_lost_result(const GameFSM *game);
void print_game_summary(const char *name, int won,
                        int guesses_used, int total_seconds);

/* -- Leaderboard -------------------------------------------------------- */
void show_leaderboard(void);

/* -- Input helpers ------------------------------------------------------ */
void read_player_name(char *name, int max_len);
void get_aesthetic_input(char *buffer, int max_len);
void prompt_return_to_menu(void);

#endif