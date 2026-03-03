#ifndef HISTORY_H
#define HISTORY_H

#include "common.h"

// 1. Add a command to the history list (with shifting logic)
void add_to_history(const char *cmd);

// 2. Write the history array to .remote_history
void save_history();

// 3. Read .remote_history into the history array
void load_history();

// 4. Format the history into a string for the client to display
void print_history(char *result);

#endif