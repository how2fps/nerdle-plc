#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "input.h"
#include "game_ui.h"
#include "leaderboard.h"
#include "parser.h"

#define MAX_GUESSES 6

/*
 * Displays a one-time welcome screen when the program starts.
 * Clears the terminal, shows the game title in cyan, a short description,
 * how-to-play rules, and the colour key for feedback tiles.
 * Waits for any keypress before returning so the player can read it.
 */
void print_intro(void)
{
       printf("\033[2J\033[H");
       printf("\n");
       printf(COLOR_CYAN COLOR_BOLD "+============================+\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "      WELCOME TO NERDLE     " COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_CYAN COLOR_BOLD "+============================+\n" COLOR_RESET);

       printf(COLOR_SILVER COLOR_BOLD "\n  Nerdle is a number-based\n" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD "  word game. Guess the hidden\n" COLOR_RESET);
       printf(COLOR_SILVER COLOR_BOLD "  equation in 6 tries!\n" COLOR_RESET);

       printf(COLOR_CYAN COLOR_BOLD "\n  HOW TO PLAY:\n" COLOR_RESET);
       printf(COLOR_SILVER "  - Each guess must be a valid\n" COLOR_RESET);
       printf(COLOR_SILVER "    equation of exactly 8 chars\n" COLOR_RESET);
       printf(COLOR_SILVER "    (e.g. 12+3=15 or 9*8=72)\n\n" COLOR_RESET);

       printf(COLOR_GREEN "  [G]" COLOR_RESET COLOR_SILVER " = correct position\n" COLOR_RESET);
       printf(COLOR_YELLOW "  [Y]" COLOR_RESET COLOR_SILVER " = wrong position\n" COLOR_RESET);
       printf(COLOR_RED "  [R]" COLOR_RESET COLOR_SILVER " = not in equation\n" COLOR_RESET);

       printf(COLOR_CYAN COLOR_BOLD "\n+============================+\n" COLOR_RESET);
       printf(COLOR_SILVER "  Press any key to continue..." COLOR_RESET);
       getch();
}

/*
 * Clears the screen and renders the main navigation menu.
 * Each menu option is colour-coded to match its associated section.
 * The player types a single character to select an option.
 */
void print_menu(void)
{
       printf("\033[2J\033[H");
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
       printf("Selection: ");
}

/*
 * Renders a centred section banner above each sub-screen (e.g. "WATCH REPLAY").
 *
 * Centering is computed from the inner box width (28 chars) and the
 * length of the title string.
 */
void print_section_header(const char *title)
{
       int title_len = (int)strlen(title);
       int total_width = 28;
       int padding = (total_width - title_len) / 2;
       int right_pad = total_width - title_len - padding;

       printf(COLOR_BLUE COLOR_BOLD "\n+============================+\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "|" COLOR_RESET);

       if (!strcmp(title, "ADD EQUATION"))
       {
              printf(COLOR_CYAN COLOR_BOLD "%*s%s%*s" COLOR_RESET, padding, "", title, right_pad, "");
       }
       else if (!strcmp(title, "CHALLENGE MODE"))
       {
              printf(COLOR_RED COLOR_BOLD "%*s%s%*s" COLOR_RESET, padding, "", title, right_pad, "");
       }
       else if (!strcmp(title, "WATCH REPLAY"))
       {
              printf(COLOR_BLUE COLOR_BOLD "%*s%s%*s" COLOR_RESET, padding, "", title, right_pad, "");
       }
       else
       {
              printf(COLOR_SILVER COLOR_BOLD "%*s%s%*s" COLOR_RESET, padding, "", title, right_pad, "");
       }

       printf(COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET);
       printf(COLOR_BLUE COLOR_BOLD "+============================+\n" COLOR_RESET);
}

/*
 * Switches the terminal to the alternate screen buffer and clears it.
 * This isolates the game board from the main menu output so that
 * returning to the menu restores the previous terminal state cleanly.
 */
void enter_game_view(void)
{
       printf("\033[?1049h\033[2J\033[H");
       fflush(stdout);
}

/*
 * Restores the terminal from the alternate screen buffer back to the
 * primary buffer, effectively returning to the state before enter_game_view().
 */
void leave_game_view(void)
{
       printf("\033[?1049l");
       fflush(stdout);
}

/*
 * Renders the full 6-row guess grid to the terminal.
 * Completed rows show each character colour-coded by its feedback:
 *   GREEN  - correct character in the correct position
 *   YELLOW - correct character in the wrong position
 *   RED    - character not present in the answer
 * Unfilled rows are shown as empty slots "[_]".
 *
 * Also prints the target equation as a debug aid (above the board).
 */
void print_guess_board(const GameFSM *game)
{
       int row;
       int col;

       if (game == NULL)
              return;

       printf("\033[H\033[J");
       printf("\nTarget equation (for debugging): %s\n\n", game->answer);
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
                     /* render a completed row with colour-coded feedback */
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
                     /* render an empty row for guesses not yet made */
                     for (col = 0; col < EQUATION_LEN; col++)
                            printf("[_]");
              }
              printf("\n");
       }
       printf("\n");
       fflush(stdout);
}

/*
 * Redraws the guess board and prints a status line below it.
 * Shows "You got the answer!" if the game has been won, or the
 * number of remaining guesses otherwise.
 */
void print_turn_status(const GameFSM *game)
{
       print_guess_board(game);
       if (is_game_won(game) == 1)
              printf("Congratulations! You got the answer!\n");
       else
              printf("Guesses left: %d\n\n", get_guesses_left(game));
}

/*
 * Prints the answer when the player has exhausted all guesses without
 * winning. Does nothing if the game was won or guesses remain.
 */
void print_game_lost_result(const GameFSM *game)
{
       if (game == NULL)
              return;
       if (is_game_won(game) != 1 && get_guesses_left(game) == 0)
              printf("No guesses left. The answer was: %s\n", game->answer);
}

/*
 * Displays a formatted end-of-game summary box showing the outcome,
 * player name, number of guesses used out of the maximum, and elapsed time.
 */
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
       printf(COLOR_BLUE COLOR_BOLD "+===========================+\n" COLOR_RESET);

       if (won)
              printf(COLOR_BLUE COLOR_BOLD "|  Result : " COLOR_GREEN "%-16s" COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET, "YOU WON!");
       else
              printf(COLOR_BLUE COLOR_BOLD "|  Result : " COLOR_RED "%-16s" COLOR_BLUE COLOR_BOLD "|\n" COLOR_RESET, "Try Again");

       printf(COLOR_BLUE COLOR_BOLD "|  Player : %-16s|\n" COLOR_RESET, name);
       printf(COLOR_BLUE COLOR_BOLD "|  Guesses: %d / %-12d|\n" COLOR_RESET, guesses_used, MAX_GUESSES);
       printf(COLOR_BLUE COLOR_BOLD "|  Time   : %02d:%02d           |\n" COLOR_RESET, minutes, seconds);
       printf(COLOR_BLUE COLOR_BOLD "+===========================+\n" COLOR_RESET);
}

/*
 * Prints the column header row for the leaderboard table.
 * Called internally by show_leaderboard() before the data rows are printed.
 */
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

/*
 * Prints the leaderboard header and then delegates to readLeaderboard()
 * to populate the data rows from the saved leaderboard file.
 * Draws a closing border after the data.
 */
void show_leaderboard(void)
{
       print_leaderboard_header();
       readLeaderboard();
       printf(COLOR_BLUE COLOR_BOLD "+======+====================+========+============+==========+\n" COLOR_RESET);
}

/* -- Input helpers ------------------------------------------------------ */
/*
 * Prompts the player to enter their name and reads it from stdin.
 * Strips the trailing newline. Falls back to "Player" if the input is empty.
 */
void read_player_name(char *name, int max_len)
{
       printf("Enter your name: ");
       fgets(name, max_len, stdin);
       name[strcspn(name, "\n")] = '\0';
       if (strlen(name) == 0)
              strcpy(name, "Player");
}

/*
 * Reads a fixed-length equation string character by character, displaying
 * a live preview with underscore placeholders for unfilled positions.
 * Supports backspace/delete for corrections.
 * Only accepts printable characters; Enter is only accepted once the
 * buffer has reached max_len.
 */
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
                     /* only accept Enter when the buffer is exactly full */
                     if (current_len == max_len)
                            break;
                     continue;
              }

              if (ch == '\b' || ch == 127)
              {
                     /* backspace: remove the last character if any */
                     if (current_len > 0)
                     {
                            current_len--;
                            buffer[current_len] = '\0';
                     }
              }
              else if (current_len < max_len && isprint(ch))
              {
                     /* append printable character to the buffer */
                     buffer[current_len] = ch;
                     current_len++;
                     buffer[current_len] = '\0';
              }
       }
       printf("\n");
}

/*
 * Displays a "press any key" message and blocks until the player
 * presses a key. Used at the end of sub-screens to pause before
 * the screen is cleared and the menu is redrawn.
 */
void prompt_return_to_menu(void)
{
       printf("Press any key to return to menu...\n");
       getch();
}