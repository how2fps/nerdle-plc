#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#define MAX_NAME_LEN 64
#define MAX_ENTRIES  100
#define LEADERBOARD_FILE "../leaderboard.txt"

typedef struct {
    char name[MAX_NAME_LEN];
    int  score;
    char date[12];   /* "DD/MM/YYYY\0" */
    char time[12];   /* "HH:MM:SS\0"  */
} LeaderboardEntry;

/* Leaderboard functions */
int loadLeaderboard(LeaderboardEntry entries[], int maxEntries);
void readLeaderboard(void);
int writeLeaderboard(const char *name, int score);

#endif