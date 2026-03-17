#include <stdio.h>
#include "file_reader.h"
#include "game_logic.h"

int main()
{
       FILE *fp;

       /*load equation list from file here*/
       char *equation;
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

       printf("%d", line_count);

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
       equation[len] = '\0'; /* we get equation here, now we scanf */

       while (attempts < max_guess && game_won != 1)
       {

              
              /*game logic*/

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