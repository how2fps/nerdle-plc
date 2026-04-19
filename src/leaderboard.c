#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "leaderboard.h"

/*
 * Opens LEADERBOARD_FILE and parses up to maxEntries records into the
 * provided entries array. Blank lines are silently skipped. Lines that
 * do not match the expected six-field format are also skipped.
 *
 * Returns the number of successfully parsed entries, or 0 if the file
 * could not be opened or contains no valid records.
 */
int loadLeaderboard(LeaderboardEntry entries[], int maxEntries)
{
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

    while (count < maxEntries && fgets(line, sizeof(line), fp))
    {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0')
            continue; /* Skip blank lines */
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

/*
 * Loads all leaderboard entries from the file and prints them as a
 * formatted table to stdout. The table includes rank, name, completion
 * time, date, and the time-of-day the entry was logged.
 *
 * Prints a message and returns early if no entries are found.
 * Called by show_leaderboard() in game_ui.c, which draws the surrounding
 * border before and after this function.
 */
void readLeaderboard(void)
{
    LeaderboardEntry entries[MAX_ENTRIES];
    int count;
    int i;

    count = loadLeaderboard(entries, MAX_ENTRIES);

    if (count == 0)
    {
        printf("No entries found. File may not exist or is empty.\n");
        return;
    }

    printf("%-6s %-20s %-8s %-12s %-10s\n", "Rank", "Name", "Time", "Date", "Logged");
    printf("%-6s %-20s %-8s %-12s %-10s\n", "----", "----", "----", "----", "------");
    for (i = 0; i < count; i++)
    {
        printf("%-6d %-20s %02d:%02d    %-12s %-10s\n",
               i + 1,
               entries[i].name,
               entries[i].minutes,
               entries[i].seconds,
               entries[i].date,
               entries[i].time);
    }
}

/*
 * Records a new winning entry into the leaderboard file, maintaining
 * ascending sort order by total completion time (fastest time = rank 1).
 *
 * Steps:
 *   1. Capture the current date and time using localtime().
 *   2. Load existing entries from the file.
 *   3. If the leaderboard is full (count >= MAX_ENTRIES), only insert
 *      the new entry if it is faster than the slowest existing entry;
 *      otherwise reject it and return 0.
 *   4. Insert the new entry at the correct sorted position using a
 *      rightward shift.
 *   5. Rewrite the entire file with updated rank numbers.
 *
 * Returns:
 *    0 - entry written successfully, or time did not qualify
 *   -1 - file could not be opened for writing
 */
int writeLeaderboard(const char *name, int minutes, int seconds)
{
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

    /* if the leaderboard is full, check whether the new time beats the last entry */
    if (count >= MAX_ENTRIES)
    {
        existing_time = entries[MAX_ENTRIES - 1].minutes * 60 + entries[MAX_ENTRIES - 1].seconds;
        new_time = minutes * 60 + seconds;
        if (new_time < entries[MAX_ENTRIES - 1].minutes * 60 + entries[MAX_ENTRIES - 1].seconds)
        {
            count = MAX_ENTRIES - 1;
        }
        else
        {
            printf("Time of %02d:%02d did not make the leaderboard.\n", minutes, seconds);
            return 0;
        }
    }

    /* find the correct insertion position using a rightward shift */
    i = count - 1;
    new_time = minutes * 60 + seconds;
    while (i >= 0)
    {
        existing_time = entries[i].minutes * 60 + entries[i].seconds;
        if (existing_time > new_time)
        {
            entries[i + 1] = entries[i];
            i--;
        }
        else
        {
            break;
        }
    }
    /* insert the new entry at the gap left by the shift */
    entries[i + 1] = newEntry;
    count++;

    /* rewrite the entire file so rank numbers stay consecutive and correct */
    fp = fopen(LEADERBOARD_FILE, "w");
    if (!fp)
    {
        fprintf(stderr, "Error opening leaderboard file for writing.\n");
        return -1;
    }

    for (i = 0; i < count; i++)
    {
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
