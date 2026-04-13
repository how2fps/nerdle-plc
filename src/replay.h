#ifndef REPLAY_H
#define REPLAY_H
#include "game_logic.h"
#include <stdio.h>
#include "leaderboard.h"
#include "game_ui.h"

void save_replay(char name[MAX_NAME_LEN], GameFSM *g);

void play_replay(void);

#endif
