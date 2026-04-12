#include <stdio.h>
#include <conio.h>
#include "game_ui.h"

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
       {
              return;
       }

       printf("\033[H\033[J");
       printf("Target equation (for debugging): %s\n", game->answer);
       printf("========= NERDLE ========\n");
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
       fflush(stdout);
}

void print_turn_status(const GameFSM *game)
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

void prompt_return_to_menu(void)
{
       printf("Press any key to return to menu...\n");
       _getch();
}

void print_game_lost_result(const GameFSM *game)
{
       if (game == NULL)
       {
              return;
       }

       if (is_game_won(game) != 1 && get_guesses_left(game) == 0)
       {
              printf("No guesses left. The answer was: %s\n", game->answer);
       }
}
