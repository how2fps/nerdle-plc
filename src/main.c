#include <stdio.h>

int main()
{

       // load equation list from file here
       int attempts = 0;
       int game_won = 0;
       int max_guess = 6;

       while (attempts < max_guess && game_won != 1)
       {
              // game logic

              attempts++;
       }

       if (game_won == 1)
       {
              // game win logic
       }
       else if (game_won == 0)
       {
              // game lose logic
       }
       else
       {
              // bug with game_won, not possible but account for it
       }

       return 0;
}