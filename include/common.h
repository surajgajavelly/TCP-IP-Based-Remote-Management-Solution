#ifndef COMMON_H
#define COMMON_H

// ANSI Color Macros
#define RESET          "\033[0m"
#define BOLD           "\033[1m"
#define RED            "\033[1;31m"
#define GREEN          "\033[1;32m"
#define YELLOW         "\033[1;33m"
#define BLUE           "\033[1;34m"
#define MAGENTA        "\033[1;35m"
#define CYAN           "\033[1;36m"
#define WHITE          "\033[1;37m"
#define GRAY           "\033[0;90m"

#define MAX_BUF 1024
#define MAX_HISTORY 50
#define DEFAULT_PORT 8080
#define HISTORY_FILE ".remote_history"

// Command IDs - The "Language" of our protocol
typedef enum {
    CMD_GET_MEM = 1,
    CMD_GET_CPU,
    CMD_GET_PORTS,
    CMD_GET_FD,
    CMD_KILL,
    CMD_HELP,
    CMD_HISTORY,
    CMD_EXIT
} command_t;

// Request Structure: What the Client sends to the Server
typedef struct {
    command_t cmd_id;          // Which command (1, 2, 3...)
    char proc_name[256];      // The process name (e.g., "firefox")
} request_t;

// Response Structure: What the Server sends back
typedef struct {
    int status;               // 0 for Success, -1 for Error
    char data[MAX_BUF];       // The actual result string
} response_t;

#endif