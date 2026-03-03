#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "history.h"
#include "common.h"

static char history[MAX_HISTORY][MAX_BUF];
static int history_count = 0;

void add_to_history(const char *cmd)
{
    if (history_count < MAX_HISTORY) {
        strncpy(history[history_count++], cmd, MAX_BUF);
    }
    else
    {
        // Shift history left and add new command at the end
        for (int i = 0; i < MAX_HISTORY - 1; i++)
        {
            strcpy(history[i], history[i + 1]);
        }
        strncpy(history[MAX_HISTORY - 1], cmd, MAX_BUF);
    }
}

void save_history() {
    FILE *f = fopen(HISTORY_FILE, "w");
    if (!f)
    {
        return;
    }
    for (int i = 0; i < history_count; i++)
    {
        fprintf(f, "%s\n", history[i]);
    }
    fclose(f);
}

void load_history() {
    FILE *f = fopen(HISTORY_FILE, "r");
    if (!f)
    {
        return;
    }

    history_count = 0;
    char line[MAX_BUF];
    while (fgets(line, MAX_BUF, f) && history_count < MAX_HISTORY)
    {
        line[strcspn(line, "\n")] = 0;
        strcpy(history[history_count++], line);
    }
    fclose(f);
}

void print_history(char *result)
{
    result[0] = '\0';
    // Increase buffer size to handle index + command + formatting
    char entry[MAX_BUF + 64]; 
    
    for (int i = 0; i < history_count; i++)
    {
        // Use snprintf for safe, bounded string formatting
        snprintf(entry, sizeof(entry), "%d  %s\n", i + 1, history[i]);
        strcat(result, entry);
    }
}