#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <signal.h>
#include "common.h"
#include "network.h"
#include "proc_utils.h"

/**
 * handle_client: Runs in the CHILD process.
 * Manages the command-response loop for a single connected client.
 */
void handle_client(int client_fd) 
{
    request_t req;
    response_t res;

    // Read the request structure directly from the socket
    while (read(client_fd, &req, sizeof(request_t)) > 0) 
    {
        printf("[Child %d] Request Received: Cmd %d for Process: %s\n", getpid(), req.cmd_id, req.proc_name);

        // 1. First, find the PID associated with the process name
        int pid = get_pid_by_name(req.proc_name);
        
        if (pid == -1)
        {
            res.status = -1;
            snprintf(res.data, MAX_BUF, "Error: Process '%s' not found.", req.proc_name);
        }
        else
        {
            res.status = 0;
            // 2. Route the command to the appropriate logic in proc_utils
            switch(req.cmd_id)
            {
                case CMD_GET_MEM:
                    get_memory_usage(pid, res.data);
                    break;
                case CMD_GET_CPU:
                    get_cpu_usage(pid, res.data);
                    break;
                case CMD_GET_PORTS:
                    get_ports(pid, res.data);
                    break;
                case CMD_GET_FD:
                    get_fds(pid, res.data);
                    break;
                case CMD_KILL:
                    // Using SIGKILL (9) for immediate termination
                    if (kill(pid, SIGKILL) == 0) 
                        snprintf(res.data, MAX_BUF, "Process %s (PID %d) killed successfully.", req.proc_name, pid);
                    else 
                        strcpy(res.data, "Error: Failed to kill process (Check permissions).");
                    break;
                default:
                    res.status = -1;
                    strcpy(res.data, "Unknown Command ID.");
            }
        }
        // 3. Send the response structure back to the client
        write(client_fd, &res, sizeof(response_t));
    }

    printf("[Child %d] Client disconnected. Closing session.\n", getpid());
    close(client_fd);
    exit(0); // Child process must exit here
}

int main(int argc, char const *argv[])
{
    int port = DEFAULT_PORT;
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }

    // Initialize the server socket (socket, bind, listen)
    int server_fd = setup_server(port);
    if (server_fd < 0)
    {
        fprintf(stderr, "Failed to initialize server.\n");
        exit(EXIT_FAILURE);
    }

    printf("--- Remote Diagnostic Server ---\n");
    printf("Listening on port %d...\n", port);

    while (1)
    {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        
        // Wait for a new client connection (Blocking call)
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) 
        {
            perror("Accept failed");
            continue;
        }

        printf("\n[+] Connection from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        // Create a child process
        pid_t pid = fork();

        if (pid == 0)
        { 
            // CHILD PROCESS LOGIC
            close(server_fd); // Child doesn't need the listener
            handle_client(new_socket);
        } 
        else if (pid > 0) 
        {
            // PARENT PROCESS LOGIC
            close(new_socket); // Parent doesn't need to talk to client
            
            // Clean up all "zombie" child processes that have finished
            while (waitpid(-1, NULL, WNOHANG) > 0);
        } 
        else
        {
            perror("Fork failed");
            close(new_socket);
        }
    }

    close(server_fd);
    return 0;
}