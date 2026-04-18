#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "input.h"
#include "game_ui.h"
#include "leaderboard.h"
#include "parser.h"

#define MAX_GUESSES 6

void print_menu(void)
{
       printf("\n");
       printf(COLOR_CYAN COLOR_BOLD "+============================+\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD "     NERDLE  GAME  MENU     " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "+============================+\n" COLOR_RESET);

       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_GREEN COLOR_BOLD "    1. Play Game            " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_YELLOW COLOR_BOLD "    2. Check Leaderboard    " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "    3. Add New Equation     " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_RED COLOR_BOLD "    4. Challenge Mode       " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "    5. Watch Replay         " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD "    6. Exit                 " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "+============================+\n" COLOR_RESET);
       printf("  Selection: ");
}

void print_section_header(const char *title)
{
       printf(COLOR_BLUE COLOR_BOLD "\n+============================+\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);

       if(!strcmp(title, "ADD EQUATION")){
              printf(COLOR_CYAN COLOR_BOLD " %-27s" COLOR_RESET, title);   
       }
       else if(!strcmp(title, "CHALLENGE MODE")){
              printf(COLOR_RED COLOR_BOLD " %-27s" COLOR_RESET, title);
       }
       else if(!strcmp(title, "WATCH REPLAY")){
              printf(COLOR_BLUE COLOR_BOLD " %-27s" COLOR_RESET, title);
       } 
       else{
              printf(COLOR_SILVER COLOR_BOLD " %-27s" COLOR_RESET, title);
       }

       printf(COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "+============================+\n" COLOR_RESET);
}

void enter_game_view(void)
{
       printf("\033[?1049h\033[2J\033[H");
       fflush(stdout);
}

void leave_game_view(void)
{
       printf("\033[?1049l");
       fflush(stdout);
}

void print_guess_board(const GameFSM *game)
{
       int row;
       int col;

       if (game == NULL)
              return;

       printf("\033[H\033[J");
       printf("Target equation (for debugging): %s\n", game->answer);
       printf(COLOR_CYAN COLOR_BOLD "+======================+\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_GREEN COLOR_BOLD "      N " COLOR_RESET);
       printf(COLOR_YELLOW COLOR_BOLD "E " COLOR_RESET);
       printf(COLOR_GREEN COLOR_BOLD "R " COLOR_RESET);
       printf(COLOR_YELLOW COLOR_BOLD "D " COLOR_RESET);
       printf(COLOR_GREEN COLOR_BOLD "L " COLOR_RESET);
       printf(COLOR_RED COLOR_BOLD "E     " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "+======================+\n" COLOR_RESET);
       for (row = 0; row < game->max_guesses; row++)
       {
              if (row < game->guesses_used)
              {
                     for (col = 0; col < EQUATION_LEN; col++)
                     {
                            if (game->feedback_history[row][col] == CORRECT)
                                   printf("[" GREEN "%c" RESET "]", game->guess_history[row][col]);
                            else if (game->feedback_history[row][col] == PARTIAL)
                                   printf("[" YELLOW "%c" RESET "]", game->guess_history[row][col]);
                            else
                                   printf("[" RED "%c" RESET "]", game->guess_history[row][col]);
                     }
              }
              else
              {
                     for (col = 0; col < EQUATION_LEN; col++)
                            printf("[_]");
              }
              printf("\n");
       }
       printf("\n");
       fflush(stdout);
}

void print_turn_status(const GameFSM *game)
{
       print_guess_board(game);
       if (is_game_won(game) == 1)
              printf("You got the answer!\n");
       else
              printf("Guesses left: %d\n\n", get_guesses_left(game));
}

void print_game_lost_result(const GameFSM *game)
{
       if (game == NULL)
              return;
       if (is_game_won(game) != 1 && get_guesses_left(game) == 0)
              printf("No guesses left. The answer was: %s\n", game->answer);
}

void print_game_summary(const char *name, int won,
                        int guesses_used, int total_seconds)
{
       int minutes = total_seconds / 60;
       int seconds = total_seconds % 60;

       printf("\n");
       printf(COLOR_BLUE COLOR_BOLD "+===========================+\n");
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD "        GAME SUMMARY       " COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD  "+===========================+\n" COLOR_RESET);

       if (won)
              printf(COLOR_BLUE COLOR_BOLD "|  Result : " COLOR_GREEN "%-16s" COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET, "YOU WON!");
       else
              printf(COLOR_BLUE COLOR_BOLD "|  Result : " COLOR_RED "%-16s" COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET, "Try Again");

       printf(COLOR_BLUE COLOR_BOLD "|  Player : %-16s|\n" COLOR_RESET, name);
       printf(COLOR_BLUE COLOR_BOLD "|  Guesses: %d / %-12d|\n" COLOR_RESET, guesses_used, MAX_GUESSES);
       printf(COLOR_BLUE COLOR_BOLD "|  Time   : %02d:%02d           |\n" COLOR_RESET, minutes, seconds);
       printf(COLOR_BLUE COLOR_BOLD "+===========================+\n" COLOR_RESET);
}

/* -- Leaderboard -------------------------------------------------------- */

static void print_leaderboard_header(void)
{
       printf(COLOR_BLUE COLOR_BOLD "+============================================================+\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD "                       LEADERBOARD                          " COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "+======+====================+========+============+==========+\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD " Rank " COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD " Player             " COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD " Time   " COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD " Date       " COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD " Logged   " COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "+======+====================+========+============+==========+\n" COLOR_RESET);
}

void show_leaderboard(void)
{
       print_leaderboard_header();
       readLeaderboard();
       printf(COLOR_BLUE COLOR_BOLD "+======+====================+========+============+==========+\n" COLOR_RESET);
}

/* -- Input helpers ------------------------------------------------------ */

void read_player_name(char *name, int max_len)
{
       printf("  Enter your name: ");
       fgets(name, max_len, stdin);
       name[strcspn(name, "\n")] = '\0';
       if (strlen(name) == 0)
              strcpy(name, "Player");
}

void get_aesthetic_input(char *buffer, int max_len)
{
       int current_len = 0;
       char ch;
       int i = 0;

       memset(buffer, 0, max_len + 1);

       while (1)
       {
              printf("\rEnter equation: ");
              printf("%s", buffer);

              for (i = 0; i < (max_len - current_len); i++)
                     printf("_");

              ch = getch();

              if (ch == '\r' || ch == '\n')
              {
                     if (current_len == max_len)
                            break;
                     continue;
              }

              if (ch == '\b' || ch == 127)
              {
                     if (current_len > 0)
                     {
                            current_len--;
                            buffer[current_len] = '\0';
                     }
              }
              else if (current_len < max_len && isprint(ch))
              {
                     buffer[current_len] = ch;
                     current_len++;
                     buffer[current_len] = '\0';
              }
       }
       printf("\n");
}

void prompt_return_to_menu(void)
{
       printf("Press any key to return to menu...\n");
       getch();
}