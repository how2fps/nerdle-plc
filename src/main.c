#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "file_reader.h"
#include "game_logic.h"
#include "game_ui.h"
#include "parser.h"
#include "evaluator.h"
#include <ctype.h>

#ifdef _WIN32
#include <conio.h>
#define getch _getch
#else
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int getch(void)
{
       struct termios oldt, newt;
       int ch;
       tcgetattr(STDIN_FILENO, &oldt);
       newt = oldt;
       newt.c_lflag &= ~(ICANON | ECHO);
       tcsetattr(STDIN_FILENO, TCSANOW, &newt);
       ch = getchar();
       tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
       return ch;
}

#endif

#define MAX_ANSWER_SIZE 8

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
              {
                     printf("_");
              }

              ch = getch();

              if (ch == '\r' || ch == '\n')
              {
                     if (current_len == max_len)
                     {
                            break;
                     }
                     continue;
              }

              if (ch == '\b')
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

int main(void)
{
       FILE *fp;
       int line_count = 0;
       int random_line;
       char line[100];
       char input[100];
       int i;
       int len;
       int ch;
       char choice;
       int running = 1;

       /* game logic variables */
       /* char equation[EQUATION_LEN + 1]; */
       GameFSM *game;
       GuessStatus status;
       char *equation;
       char *guess;
       char guess_input[MAX_ANSWER_SIZE + 3];
       int guess_len;
       int max_guesses = 6;

       while (running)
       {
              printf("\n=== MATH GAME MENU ===\n");
              printf("1. Play Game\n");
              printf("2. Check Leaderboard\n");
              printf("3. Add New Equation\n");
              printf("4. Challenge Mode\n");
              printf("5. Exit\n");
              printf("Selection: ");

              choice = getch();
              printf("%c\n", choice);

              switch (choice)
              {
              case '1':
                     /* printf("\n--- Starting Game ---\n"); */
                     line_count = 0;
                     srand(time(NULL));
                     fp = read_file("equations.txt");
                     while (fgets(line, sizeof(line), fp))
                     {
                            line_count++;
                     }

                     random_line = rand() % line_count;
                     rewind(fp);

                     for (i = 0; i <= random_line; i++)
                     {
                            if (fgets(line, sizeof(line), fp) == NULL)
                            {
                                   break;
                            }
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
                     {
                            return 1;
                     }

                     strncpy(equation, line, len);
                     equation[len] = '\0';

                     game = create_game(equation, max_guesses, 0);
                     if (game == NULL || game->answer == NULL)
                     {
                            free(equation);
                            destroy_game(game);
                            return 1;
                     }

                     guess = (char *)malloc(sizeof(char) * (MAX_ANSWER_SIZE + 1));
                     if (guess == NULL)
                     {
                            free(equation);
                            destroy_game(game);
                            return 1;
                     }

                     enter_game_view();
                     print_turn_status(game);
                     while (get_guesses_left(game) > 0 && is_game_won(game) != 1)
                     {
                            printf("Your guess: ");
                            if (fgets(guess_input, sizeof(guess_input), stdin) == NULL)
                            {
                                   break;
                            }

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
                                   printf("Invalid equation! Must be %d characters.\n", EQUATION_LEN);
                                   continue;
                            }

                            /* for users to return to main menu from selection 1 gracefully */
                            if (!strcmp(guess_input, "q"))
                            {
                                   break;
                            }

                            strncpy(guess, guess_input, MAX_ANSWER_SIZE);
                            guess[MAX_ANSWER_SIZE] = '\0';

                            status = play_guess_turn(game, guess);
                            if (status == GUESS_INVALID)
                            {
                                   continue;
                            }

                            if (status == GUESS_ERROR)
                            {
                                   printf("Could not evaluate guess. Try again.\n");
                                   continue;
                            }

                            print_turn_status(game);
                     }

                     if (is_game_won(game) == 1)
                     {
                            prompt_return_to_menu();
                     }

                     leave_game_view();
                     print_game_lost_result(game);

                     free(guess);
                     free(equation);
                     destroy_game(game);

                     break;

              case '2':
                     printf("\n--- Leaderboard ---\n");
                     break;

              case '3':
                     get_aesthetic_input(input, EQUATION_LEN);
                     if (validate_equation(input) && process_line(input, 1))
                     {
                            int duplicate = 0;
                            char file_line[100];
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
                                   printf("Equation already exists!\n");
                            }
                            else
                            {
                                   fp = fopen("equations.txt", "a");
                                   if (fp == NULL)
                                   {
                                          printf("Error: Could not open equations.txt\n");
                                          break;
                                   }
                                   fprintf(fp, "%s\n", input);
                                   fclose(fp);
                                   printf("Equation added!\n");
                            }
                     }
                     else
                     {
                            printf("Invalid equation. Not added.\n");
                     }
                     break;

              case '4':
                     printf("\n--- Challenge Mode ---\n");
                     do
                     {
                            get_aesthetic_input(input, EQUATION_LEN);
                     } while (validate_equation(input) == 0 || process_line(input, 1) == 0);

                     len = strlen(input);
                     if (len > 0 && input[len - 1] == '\n')
                     {
                            input[len - 1] = '\0';
                            len--;
                     }

                     equation = (char *)malloc(sizeof(char) * (len + 1));
                     if (equation == NULL)
                     {
                            return 1;
                     }

                     strncpy(equation, input, len);
                     equation[len] = '\0';

                     printf("\033[H\033[J");
                     game = create_game(equation, max_guesses, 0);
                     if (game == NULL || game->answer == NULL)
                     {
                            free(equation);
                            destroy_game(game);
                            return 1;
                     }

                     guess = (char *)malloc(sizeof(char) * (MAX_ANSWER_SIZE + 1));
                     if (guess == NULL)
                     {
                            free(equation);
                            destroy_game(game);
                            return 1;
                     }

                     printf("Start guessing!\n");
                     while (get_guesses_left(game) > 0 && is_game_won(game) != 1)
                     {
                            printf("Your guess: ");
                            if (fgets(guess_input, sizeof(guess_input), stdin) == NULL)
                            {
                                   break;
                            }

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
                                   printf("Invalid equation! Must be %d characters.\n", EQUATION_LEN);
                                   continue;
                            }

                            strncpy(guess, guess_input, MAX_ANSWER_SIZE);
                            guess[MAX_ANSWER_SIZE] = '\0';

                            if (game->current_state == GAME_STATE_START)
                            {
                                   transition_gamestate(game, GAME_EVENT_INIT); /* to GAME_STATE_INPUT  */
                            }
                            transition_gamestate(game, GAME_EVENT_SUBMIT_GUESS); /* to GAME_STATE_VALIDATION  */

                            status = play_guess_turn(game, guess);
                            if (status == GUESS_INVALID)
                            {
                                   continue;
                            }

                            if (status == GUESS_ERROR)
                            {
                                   printf("Could not evaluate guess. Try again.\n");
                                   continue;
                            }
                     }

                     if (is_game_won(game) != 1 && get_guesses_left(game) == 0)
                     {
                            printf("No guesses left. The answer was: %s\n", game->answer);
                     }

                     free(guess);
                     free(equation);
                     destroy_game(game);

                     break;

              case '5':
                     printf("Goodbye!\n");
                     return 0;

              default:
                     printf("Invalid choice. Please try again.\n");
                     break;
              }
       }

       return 0;
}
