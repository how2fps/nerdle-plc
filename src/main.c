#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "file_reader.h"
#include "game_logic.h"
#include "parser.h"
#include "evaluator.h"

#define MAX_ANSWER_SIZE 8
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

int main()
{
       FILE *fp;

       /*load equation list from file here*/
       char *equation;
       char *guess;
       int attempts = 0;
       int game_won = 0;
       int max_guess = 6;
       int line_count = 0;
       int random_line;
       /*char equation[9];*/
       char line[100];

       char input[100];
       int i;
       int len;
       int choice = 0;
       int running = 1;

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
                     /* Call your game logic here */
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

       GameFSM *game;

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
       printf("%s\n", equation);

       game = create_game(equation);
       if (game == NULL || game->answer == NULL)
       {
              free(equation);
              if (game != NULL)
              {
                     free(game);
              }
                     return 1;
       }

       guess = (char *)malloc(sizeof(char) * (MAX_ANSWER_SIZE + 1));
       if (guess == NULL)
       {
              return 1;
       }

       printf("Start guessing!\n");
       while (attempts < max_guess && game_won != 1)
       {
              scanf(" %8s", guess);
              validate_userinput(game, guess);

              /* if (validate_equation(guess) == 0)
              {
                     printf("Invalid equation! Try again.\n");
                     continue;  don't consume an attempt 
              }
              if (strncmp(guess, equation, EQUATION_LEN) == 0)
              {
                     printf("You got the answer!");
                     break;
              }; */

              attempts++;
       }

       if (game_won == 1)

       {
              /* game win logic */
       }
       else if (game_won == 0)
       {
              /* game lose logic */
       }
       else
       {
              /* bug with game_won, not possible but account for it */
       }

       return 0;
}