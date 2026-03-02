#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
int setup_server(int port);
int connect_to_server(const char *ip, int port);

#endif