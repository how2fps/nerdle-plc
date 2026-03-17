#include <stdio.h>
#include "file_reader.h"
#include "game_logic.h"

int main()
{
       FILE *fp;

       /*load equation list from file here*/
       int attempts = 0;
       int game_won = 0;
       int max_guess = 6;
       int line_count = 0;
       /*char equation[9];
       GameState game;*/
       char line[100];
       fp = read_file("equations.txt");
       while (fgets(line, sizeof(line), fp))
       {
              line_count++;
       }

       printf("%d", line_count);

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