#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "network.h"

// External declarations (Implemented in history.c)
extern void add_to_history(const char *cmd);
extern void save_history();
extern void load_history();
extern void print_history(char *result);

/**
 * parse_command: Converts user string input into the protocol Enum IDs.
 */
command_t parse_command(char *cmd_str) {
    if (!cmd_str) return 0;
    if (strcmp(cmd_str, "get-mem") == 0)        return CMD_GET_MEM;
    if (strcmp(cmd_str, "get-cpu-usage") == 0)  return CMD_GET_CPU;
    if (strcmp(cmd_str, "get-ports-used") == 0) return CMD_GET_PORTS;
    if (strcmp(cmd_str, "get-open-fd") == 0)    return CMD_GET_FD;
    if (strcmp(cmd_str, "kill") == 0)           return CMD_KILL;
    if (strcmp(cmd_str, "help") == 0)           return CMD_HELP;
    if (strcmp(cmd_str, "history") == 0)        return CMD_HISTORY;
    if (strcmp(cmd_str, "exit") == 0)           return CMD_EXIT;
    return 0;
}

void display_help() {
    printf("\n%s========================================================%s\n", CYAN, RESET);
    printf("%s             Remote Management Help Menu%s\n", RED, RESET);
    printf("%s========================================================%s\n", CYAN, RESET);
    printf("Usage: %s<command>%s %s[process-name]%s\n\n", GREEN, RESET, YELLOW, RESET);

    printf("%s[Process Info]%s\n", MAGENTA, RESET);
    printf("%s--------------------------------------------------------%s\n", GRAY, RESET);
    printf("  %sget-mem%s %s<proc>%s        %s->%s Fetch memory usage (VmRSS)\n", GREEN, RESET, YELLOW, RESET, BLUE, RESET);
    printf("  %sget-cpu-usage%s %s<proc>%s  %s->%s Fetch CPU ticks delta\n", GREEN, RESET, YELLOW, RESET, BLUE, RESET);
    printf("  %sget-ports-used%s %s<proc>%s %s->%s Identify socket file descriptors\n", GREEN, RESET, YELLOW, RESET, BLUE, RESET);
    printf("  %sget-open-fd%s %s<proc>%s    %s->%s Count open file descriptors\n\n", GREEN, RESET, YELLOW, RESET, BLUE, RESET);

    printf("%s[Process Control]%s\n", MAGENTA, RESET);
    printf("%s--------------------------------------------------------%s\n", GRAY, RESET);
    printf("  %skill%s %s<proc>%s           %s->%s Terminate process (SIGKILL)\n\n", RED, RESET, YELLOW, RESET, BLUE, RESET);

    printf("%s[General]%s\n", MAGENTA, RESET);
    printf("%s--------------------------------------------------------%s\n", GRAY, RESET);
    printf("  %shistory%s               %s->%s Show command history\n", GREEN, RESET, BLUE, RESET);
    printf("  %shelp%s                  %s->%s Show this menu\n", GREEN, RESET, BLUE, RESET);
    printf("  %sexit%s                  %s->%s Disconnect and close\n", GREEN, RESET, BLUE, RESET);
    printf("%s========================================================%s\n\n", CYAN, RESET);
}


int main(int argc, char const *argv[]) {
    // 1. Connection Setup
    if (argc < 2) {
        printf("Usage: %s <Server_IP> [Port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip = argv[1];
    int port = (argc == 3) ? atoi(argv[2]) : DEFAULT_PORT;

    int sock = connect_to_server(ip, port);
    if (sock < 0) {
        fprintf(stderr, "Fatal: Connection failed.\n");
        exit(EXIT_FAILURE);
    }

    // 2. Initialize UI Environment
    load_history();
    printf("Successfully connected to agent at %s:%d\n", ip, port);
    printf("Type 'help' to see available diagnostic tools.\n");

    char input[MAX_BUF];
    char raw_input[MAX_BUF]; 
    request_t req;
    response_t res;

    // 3. Main UI Loop
    while (1)
    {
        printf("%sClient] %s", GREEN, RESET);
        fflush(stdout);

        if (fgets(input, MAX_BUF, stdin) == NULL)
        {
            break;
        }
        
        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        // Skip empty inputs
        if (strlen(input) == 0)
        {
            continue;
        }

        // 4. Local Logic (History & Help)
        if (strcmp(input, "help") == 0)
        {
            display_help();
            add_to_history(input); // Log help command to history
            save_history();
            continue;
        }

        if (strcmp(input, "history") == 0)
        {
            char hist_data[MAX_BUF * 10]; // Large buffer for history output
            print_history(hist_data);
            printf("%s", hist_data);
            add_to_history(input);
            save_history();
            continue;
        }

        // Add regular commands to history and save to .remote_history
        add_to_history(input);
        save_history();

        // 5. Tokenize Input (Command + Argument)
        strcpy(raw_input, input);
        char *cmd_token = strtok(raw_input, " ");
        char *arg_token = strtok(NULL, " ");

        req.cmd_id = parse_command(cmd_token);
        if (arg_token)
        {
            strncpy(req.proc_name, arg_token, sizeof(req.proc_name) - 1);
            req.proc_name[sizeof(req.proc_name) - 1] = '\0';
        }
        else
        {
            req.proc_name[0] = '\0';
        }

        // Handle Exit
        if (req.cmd_id == CMD_EXIT)
        {
            printf("Disconnecting...\n");
            break;
        }

        // 6. Network Communication
        if (req.cmd_id == 0)
        {
            printf("%s[Error]:%s Unknown command: '%s'.\n", RED, RESET, cmd_token);
            continue;
        }

        // Send binary request structure
        if (write(sock, &req, sizeof(request_t)) < 0)
        {
            perror("Write to server failed");
            break;
        }

        // Read binary response structure
        if (read(sock, &res, sizeof(response_t)) > 0)
        {
            if (res.status == 0)
            {
                printf("%s>>> %s%s\n", CYAN, RESET, res.data);
            } else
            {
                printf("%s[Remote Error]: %s%s\n", RED, res.data, RESET);
            }
        }
        else
        {
            printf("%s[!] Server connection lost.%s\n", RED, RESET);
            break;
        }
    }

    close(sock);
    return 0;
}