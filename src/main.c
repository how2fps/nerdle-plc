#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "file_reader.h"
#include "game_logic.h"

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
       char *answer;
       int attempts = 0;
       int game_won = 0;
       int max_guess = 6;
       int line_count = 0;
       int random_line;
       /*char equation[9];
       GameState game;*/
       char line[100];
       int i;
       int len;

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
              if (strncmp(answer, equation, 8) == 0)
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