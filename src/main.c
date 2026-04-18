#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "file_reader.h"
#include "game_logic.h"
#include "parser.h"
#include "evaluator.h"
#include "leaderboard.h"
#include "game_ui.h"
#include "replay.h"
#include "input.h"

#define MAX_ANSWER_SIZE 8
#define MAX_GUESSES 6

/*
 * Runs a complete game session for the given player and equation.
 * Creates a GameFSM, then loops reading guesses from stdin until the
 * player wins, runs out of guesses, or types "q" to quit.
 *
 * After the session ends:
 *   - Prints the game summary (and the answer if the player lost).
 *   - Writes a leaderboard entry on a win.
 *   - Saves a replay file.
 *   - Returns to the caller so main() can redraw the menu.
 */
static void run_game_session(const char *name, char *equation)
{
       GameFSM *game;
       GuessStatus status;
       char *guess;
       char guess_input[MAX_ANSWER_SIZE + 3];
       int guess_len;
       int ch;
       time_t game_start, game_end;
       int total_seconds;
       int guesses_used;

       game = create_game(equation, MAX_GUESSES, 0);
       if (game == NULL || game->answer == NULL)
       {
              free(equation);
              destroy_game(game);
              return;
       }

       guess = (char *)malloc(sizeof(char) * (MAX_ANSWER_SIZE + 1));
       if (guess == NULL)
       {
              free(equation);
              destroy_game(game);
              return;
       }

       game_start = time(NULL);
       enter_game_view();
       print_guess_board(game);

       /* main guess loop: continue until won, out of guesses, or player quits */
       while (get_guesses_left(game) > 0 && is_game_won(game) != 1)
       {
              printf("Your guess: ");
              if (fgets(guess_input, sizeof(guess_input), stdin) == NULL)
                     break;

              guess_len = (int)strcspn(guess_input, "\n");
              if (guess_input[guess_len] == '\n')
              {
                     guess_input[guess_len] = '\0';
              }
              else
              {
                     while ((ch = getchar()) != '\n' && ch != EOF)
                     {
                     }
                     printf(COLOR_RED "[!!] Input too long - must be %d characters.\n" COLOR_RESET, EQUATION_LEN);
                     printf("\n");
                     continue;
              }

              /* "q" lets the player abandon the current session */
              if (!strcmp(guess_input, "q"))
                     break;

              strncpy(guess, guess_input, MAX_ANSWER_SIZE);
              guess[MAX_ANSWER_SIZE] = '\0';

              status = play_guess_turn(game, guess);
              if (status == GUESS_INVALID)
              {
                     printf("\n");
                     continue;
              }
              if (status == GUESS_ERROR)
              {
                     printf(COLOR_RED "[!!] Could not evaluate guess. Try again.\n" COLOR_RESET);
                     printf("\n");
                     continue;
              }

              print_turn_status(game);
       }

       /* calculate elapsed time and guesses used */
       game_end = time(NULL);
       total_seconds = (int)difftime(game_end, game_start);
       guesses_used = MAX_GUESSES - get_guesses_left(game);

       if (is_game_won(game) == 1)
       {
              print_game_summary(name, 1, guesses_used, total_seconds);
              writeLeaderboard(name, total_seconds / 60, total_seconds % 60);
       }
       else if (get_guesses_left(game) == 0)
       {
              /* player ran out of guesses — reveal the answer */
              print_game_lost_result(game);
              print_game_summary(name, 0, guesses_used, total_seconds);
       }

       prompt_return_to_menu();
       save_replay(name, game);
       leave_game_view();

       free(guess);
       destroy_game(game);
}

int main(void)
{
       FILE *fp;
       int line_count = 0;
       int random_line;
       char line[100];
       char input[100];
       char name[MAX_NAME_LEN];
       int i;
       int len;
       char choice;
       int duplicate;
       char file_line[100];
       int validation_status;
       int running = 1;
       char *equation;

       /* show the welcome/how-to-play screen once at startup */
       print_intro();

       while (running)
       {
              print_menu();

              choice = getch();
              printf("%c\n", choice);

              switch (choice)
              {
              /* ---- Option 1: Play Game ---------------------------------- */
              case '1':
                     print_section_header("STARTING GAME");
                     read_player_name(name, MAX_NAME_LEN);

                     line_count = 0;
                     srand(time(NULL));
                     fp = read_file("equations.txt");
                     if (fp == NULL)
                     {
                            printf(COLOR_RED "Error: Could not open equations.txt\n" COLOR_RESET);
                            break;
                     }
                     while (fgets(line, sizeof(line), fp))
                            line_count++;

                     random_line = rand() % line_count;
                     rewind(fp);

                     for (i = 0; i <= random_line; i++)
                     {
                            if (fgets(line, sizeof(line), fp) == NULL)
                                   break;
                     }
                     fclose(fp);

                     len = strlen(line);
                     if (len > 0 && line[len - 1] == '\n')
                     {
                            line[len - 1] = '\0';
                            len--;
                     }

                     equation = (char *)malloc(sizeof(char) * (len + 1));
                     if (equation == NULL)
                            return 1;

                     strncpy(equation, line, len);
                     equation[len] = '\0';

                     run_game_session(name, equation);
                     free(equation);
                     break;

              /* ---- Option 2: Leaderboard -------------------------------- */
              case '2':
                     show_leaderboard();
                     prompt_return_to_menu();
                     break;

              /* ---- Option 3: Add New Equation --------------------------- */
              case '3':
                     print_section_header("ADD EQUATION");
                     get_aesthetic_input(input, EQUATION_LEN);
                     validation_status = process_line(input, 1);

                     if (validation_status == -1)
                            break;

                     if (validate_equation(input) && validation_status == 1)
                     {
                            duplicate = 0;
                            fp = fopen("equations.txt", "r");
                            if (fp != NULL)
                            {
                                   while (fgets(file_line, sizeof(file_line), fp))
                                   {
                                          file_line[strcspn(file_line, "\n")] = '\0';
                                          if (strcmp(file_line, input) == 0)
                                          {
                                                 duplicate = 1;
                                                 break;
                                          }
                                   }
                                   fclose(fp);
                            }

                            if (duplicate)
                            {
                                   printf(COLOR_YELLOW "Equation already exists!\n" COLOR_RESET);
                                   prompt_return_to_menu();
                            }
                            else
                            {
                                   fp = fopen("equations.txt", "a");
                                   if (fp == NULL)
                                   {
                                          printf(COLOR_RED "Error: Could not open equations.txt\n" COLOR_RESET);
                                          break;
                                   }
                                   fprintf(fp, "%s\n", input);
                                   fclose(fp);
                                   printf(COLOR_GREEN "Equation added!\n" COLOR_RESET);
                                   prompt_return_to_menu();
                            }
                     }
                     else
                     {
                            printf(COLOR_RED "Invalid equation. Not added.\n" COLOR_RESET);
                            prompt_return_to_menu();
                     }
                     break;

              /* ---- Option 4: Challenge Mode ----------------------------- */
              case '4':
                     print_section_header("CHALLENGE MODE");
                     do
                     {
                            get_aesthetic_input(input, EQUATION_LEN);
                            validation_status = process_line(input, 1);
                     } while (validation_status == -1 || validate_equation(input) == 0 || validation_status == 0);

                     read_player_name(name, MAX_NAME_LEN);

                     len = strlen(input);
                     if (len > 0 && input[len - 1] == '\n')
                     {
                            input[len - 1] = '\0';
                            len--;
                     }

                     equation = (char *)malloc(sizeof(char) * (len + 1));
                     if (equation == NULL)
                            return 1;

                     strncpy(equation, input, len);
                     equation[len] = '\0';

                     run_game_session(name, equation);
                     free(equation);
                     break;

              /* ---- Option 5: Watch Replay ------------------------------- */
              case '5':
                     print_section_header("WATCH REPLAY");
                     play_replay();
                     break;

              /* ---- Option 6: Exit --------------------------------------- */
              case '6':
                     printf(COLOR_CYAN COLOR_BOLD "\n Goodbye! Thanks for playing!\n" COLOR_RESET);
                     running = 0;
                     break;

              default:
                     printf(COLOR_RED "Invalid choice. Please try again.\n" COLOR_RESET);
                     break;
              }
       }

       return 0;
}