#include "replay.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
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

       snprintf(filename, sizeof(filename), "replays/replay_%s_%ld.nrdl", name, (long)seconds);

       fptr = fopen(filename, "wb");
       if (fptr == NULL)
       {
              return;
       }

       fwrite("NRDL", 1, 4, fptr);
       fwrite(g->answer, sizeof(char), EQUATION_LEN + 1, fptr);
       fwrite(&g->guesses_used, sizeof(int), 1, fptr);
       for (i = 0; i < g->guesses_used; i++)
       {
              fwrite(g->guess_history[i], sizeof(char), EQUATION_LEN + 1, fptr);
              fwrite(g->feedback_history[i], sizeof(SlotState), EQUATION_LEN, fptr);
       }

       fclose(fptr);
}

void play_replay(void)
{
       int max_guesses = 6;
       char check[4];
       char filepath[256];
       char answer[EQUATION_LEN + 1];
       char files[32][128];
       GameFSM *g;

       int guesses_used;
       int count = 0;
       int choice = 0;
       int i = 0;
       int j = 0;
       FILE *fptr;

#ifdef _WIN32
       WIN32_FIND_DATA fd;
       HANDLE h = FindFirstFile("replays/*.nrdl", &fd);
       if (h != INVALID_HANDLE_VALUE)
       {
              do
              {
                     strncpy(files[count++], fd.cFileName, 127);
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
                            strncpy(files[count++], entry->d_name, 127);
                     }
              }
              files[count][127] = '\0';
              closedir(dir);
       }
#endif
       if (count == 0)
       {
              printf("No replays yet.");
              return;
       }
       for (j = 0; j < count; j++)
       {
              printf("%d. %s\n", j + 1, files[j]);
       }
       printf("Choose a replay (1-%d): ", count);
       scanf("%d", &choice);
       while (getchar() != '\n')
              ;
       if (choice < 1 || choice > count)
       {
              printf("Selected invalid file, please put in the position of the replay you want to play");
              return;
       }
       sprintf(filepath, "replays/%s", files[choice - 1]);

       fptr = fopen(filepath, "rb");
       if (fptr == NULL)
       {
              printf("Issue opening file");
              return;
       }

       /* Reading for NRDL here to check for no corruption, maybe change to checksum */
       fread(check, 1, 4, fptr);
       if (memcmp(check, "NRDL", 4) != 0)
       {
              printf("Invalid replay file.\n");
              fclose(fptr);
              return;
       }

       /* Reading for answer here */
       fread(answer, 1, EQUATION_LEN + 1, fptr);

       fread(&guesses_used, sizeof(int), 1, fptr);
       printf("guesses_used: %d\n", guesses_used);

       g = create_game(answer, max_guesses, 0);

       if (g == NULL)
       {
              fclose(fptr);
              return;
       }
       printf("Valid file");

       g->guesses_used = guesses_used;
       for (i = 0; i < guesses_used; i++)
       {
              fread(g->guess_history[i], sizeof(char), EQUATION_LEN + 1, fptr);
              fread(g->feedback_history[i], sizeof(SlotState), EQUATION_LEN, fptr);
       }
       fclose(fptr);

       g->guesses_used = 0;
       enter_game_view();
       print_guess_board(g);
       for (i = 1; i <= guesses_used; i++)
       {
              printf("Press any key for next guess...\n");
              getch();
              g->guesses_used = i;
              print_guess_board(g);
       }
       printf("Replay finished. Press any key to return...\n");
       getch();
       leave_game_view();
       destroy_game(g);
}
