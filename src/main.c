#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "file_reader.h"
#include "game_logic.h"
#include "parser.h"
#include "evaluator.h"
#include <conio.h>
#include <ctype.h>

#define MAX_ANSWER_SIZE 8
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"
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

              ch = _getch();

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
int main()
{
       FILE *fp;

       char equation[EQUATION_LEN + 1];
       char *answer;
       int attempts = 0;
       int game_won = 0;
       int max_guess = 6;
       int line_count = 0;
       int random_line;
       /*char equation[9];
       GameState game;*/
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
              {
                     break;
              }

              choice = atoi(input);

              switch (choice)
              {
              case 1:
                     printf("\n--- Starting Game ---\n");
                     break;

              case 2:
                     printf("\n--- Leaderboard ---\n");
                     break;

              case 3:
                     get_aesthetic_input(input, EQUATION_LEN);
                     if (validate_equation(input))
                     {
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

       strncpy(equation, line, len);
       equation[len] = '\0';

       answer = (char *)malloc(sizeof(char) * (MAX_ANSWER_SIZE + 1));
       if (answer == NULL)
       {
              return 1;
       }
       printf("%s\n", equation);
       printf("Start guessing!\n");
       while (attempts < max_guess && game_won != 1)
       {
              scanf(" %8s", answer);
              if (validate_equation(answer) == 0)
              {
                     printf("Invalid equation! Try again.\n");
                     continue; /* don't consume an attempt */
              }
              if (strncmp(answer, equation, EQUATION_LEN) == 0)
              {
                     printf("You got the answer!");
                     break;
              };

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