#include "replay.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

void save_replay(char name[MAX_NAME_LEN], GameFSM *g)
{
       int i = 0;
       FILE *fptr;
       time_t seconds;
       char filename[128];
       time(&seconds);
#ifdef _WIN32
       system("mkdir replays 2>nul");
#else
       system("mkdir -p replays");
#endif

       sprintf(filename, "replays/replay_%s_%ld.nrdl", name, (long)seconds);

       fptr = fopen(filename, "wb");
       if (fptr == NULL)
       {
              return;
       }

       fwrite("NRDL", 1, 4, fptr);
       fwrite(&g->guesses_used, sizeof(int), 1, fptr);
       for (i = 0; i < g->guesses_used; i++)
       {
              fwrite(g->guess_history[i], sizeof(char), EQUATION_LEN + 1, fptr);
              fwrite(g->feedback_history[i], sizeof(SlotState), EQUATION_LEN, fptr);
       }

       fclose(fptr);
}

void play_replay()
{
       char files[32][128];
       int count = 0;
       int choice = 0;
       int j = 0;

#ifdef _WIN32
       WIN32_FIND_DATA fd;
       HANDLE h = FindFirstFile("replays/*.nrdl", &fd);
       if (h != INVALID_HANDLE_VALUE)
       {
              do
              {
                     printf("%s\n", fd.cFileName);
              } while (FindNextFile(h, &fd));
              FindClose(h);
       }
#else
       DIR *dir = opendir("replays");
       struct dirent *entry;
       if (dir != NULL)
       {
              while ((entry = readdir(dir)) != NULL)
              {
                     if (strstr(entry->d_name, ".nrdl") != NULL)
                     {
                            printf("%s\n", entry->d_name);
                     }
              }
              closedir(dir);
       }
#endif
}