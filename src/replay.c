#include "replay.h"
#include <time.h>

void save_replay(char name[MAX_NAME_LEN], GameFSM *g)
{
       int i = 0;
       FILE *fptr;
       time_t seconds;
       char *filename;
       time(&seconds);

       sprintf(filename, "replay_%s_%ld.nrdl", name, (long)seconds);

       fptr = fopen(filename, "wb");
       for (i = 0; i < g->guesses_used - 1; i++)
       {
              fptr
       }
}
