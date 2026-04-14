#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "leaderboard.h"

/* loads leaderboard.txt file into array */
int loadLeaderboard(LeaderboardEntry entries[], int maxEntries) {
    FILE *fp;
    int count;
    int rank;
    int parsed;
    char line[256];

    fp = fopen(LEADERBOARD_FILE, "r");
    if (!fp)
        return 0;

    count = 0;
    rank = 0;

    while (count < maxEntries && fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;  /* Skip blank lines */
        parsed = sscanf(line, "%d %63s %d:%d %11s %11s",
                        &rank,
                        entries[count].name,
                        &entries[count].minutes,
                        &entries[count].seconds,
                        entries[count].date,
                        entries[count].time);
        if (parsed == 6)
            count++;
    }

    fclose(fp);
    return count;
}

/* Loads and displays the leaderboard table */
void readLeaderboard(void) {
    LeaderboardEntry entries[MAX_ENTRIES];
    int count;
    int i;

    count = loadLeaderboard(entries, MAX_ENTRIES);

    if (count == 0) {
        printf("No entries found. File may not exist or is empty.\n");
        return;
    }

    printf("%-6s %-20s %-8s %-12s %-10s\n", "Rank", "Name", "Time", "Date", "Logged");
    printf("%-6s %-20s %-8s %-12s %-10s\n", "----", "----", "----", "----", "------");
    for (i = 0; i < count; i++) {
        printf("%-6d %-20s %02d:%02d    %-12s %-10s\n",
               i + 1,
               entries[i].name,
               entries[i].minutes,
               entries[i].seconds,
               entries[i].date,
               entries[i].time);
    }
}

/* Writes a new entry to the leaderboard, sorted by score */
int writeLeaderboard(const char *name, int minutes, int seconds) {
    FILE *fp;
    time_t now;
    struct tm *t;
    LeaderboardEntry entries[MAX_ENTRIES];
    LeaderboardEntry newEntry;
    int i;
    int count;
    int new_time;
    int existing_time;
    
    now = time(NULL);
    t = localtime(&now);
    strftime(newEntry.date, 11, "%d/%m/%Y", t);
    strftime(newEntry.time, 9, "%H:%M:%S", t);
    strncpy(newEntry.name, name, MAX_NAME_LEN - 1);
    newEntry.name[MAX_NAME_LEN - 1] = '\0';
    newEntry.minutes = minutes;
    newEntry.seconds = seconds;

    count = loadLeaderboard(entries, MAX_ENTRIES);

    if (count >= MAX_ENTRIES) {
        existing_time = entries[MAX_ENTRIES - 1].minutes * 60 + entries[MAX_ENTRIES - 1].seconds;
        new_time = minutes * 60 + seconds;
        if (new_time > entries[MAX_ENTRIES - 1].minutes * 60 + entries[MAX_ENTRIES - 1].seconds) {
            count = MAX_ENTRIES - 1;
        } else {
            printf("Time of %02d:%02d did not make the leaderboard.\n", minutes, seconds);
            return 0;
        }
    }

    i = count - 1;
    new_time = minutes * 60 + seconds;
    while (i >= 0) { 
        existing_time = entries[i].minutes * 60 + entries[i].seconds;
        if (existing_time > new_time) {
            entries[i + 1] = entries[i];
            i--;
        }
        else {
            break;
        }
    }
    entries[i + 1] = newEntry;
    count++;

    fp = fopen(LEADERBOARD_FILE, "w");
    if (!fp) {
        fprintf(stderr, "Error opening leaderboard file for writing.\n");
        return -1;
    }

    for (i = 0; i < count; i++) {
        fprintf(fp, "%d %s %02d:%02d %s %s\n",
                i + 1,
                entries[i].name,
                entries[i].minutes,
                entries[i].seconds,
                entries[i].date,
                entries[i].time);
    }

    printf("'%s' added to leaderboard with time %02d:%02d.\n", name, minutes, seconds);
    fclose(fp);
    return 0;
}

/*----------------------------------------Main-----------------------------------
int main(void) {
    readLeaderboard();
    writeLeaderboard("WeiYang", 50);
    return 0;
}
    */
