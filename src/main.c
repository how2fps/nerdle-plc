#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "file_reader.h"
#include "game_logic.h"
#include "parser.h"
#include "evaluator.h"

#define MAX_ANSWER_SIZE 8

int main()
{
       FILE *fp;
       GameFSM *game;

       /*load equation list from file here*/
       char *equation;
       char *guess;
       int max_guesses = 6;
       int line_count = 0;
       int random_line;
       char line[100];

       char input[100];
       char guess_input[MAX_ANSWER_SIZE + 3];
       int i;
       int len;
       int guess_len;
       int ch;
       int choice = 0;
       int running = 1;
       GuessStatus status;

       while (running)
       {
              printf("\n=== MATH GAME MENU ===\n");
              printf("1. Play Game\n");
              printf("2. Check Leaderboard\n");
              printf("3. Add New Equation\n");
              printf("4. Exit\n");
              printf("Selection: ");

              if (fgets(input, sizeof(input), stdin) == NULL)
                     break;
              choice = atoi(input);

              switch (choice)
              {
              case 1:
                     printf("\n--- Starting Game ---\n");
                     line_count = 0;
                     /* Call your game logic here */
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
                     /* printf("%s\n", equation); */

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

              case 2:
                     printf("\n--- Leaderboard ---\n");
                     /* Call leaderboard display function */
                     break;

              case 3:
                     printf("\n--- Add New Equation ---\n");
                     printf("Enter equation (e.g., 5+2=7): ");
                     if (fgets(input, sizeof(input), stdin) != NULL)
                     {
                            /* Strip the newline character if present */
                            input[strcspn(input, "\n")] = 0;
                            process_line(input);
                     }
                     break;

              case 4:
                     printf("Goodbye!\n");
                     running = 0;
                     break;

              default:
                     printf("Invalid choice. Please try again.\n");
                     break;
              }
       }

       

       return 0;
}