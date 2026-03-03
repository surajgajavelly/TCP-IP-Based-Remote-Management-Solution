#ifndef PROC_UTILS_H
#define PROC_UTILS_H

#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include <stdlib.h>


// Function prototypes
int get_pid_by_name(const char *proc_name);

void get_memory_usage(int pid, char *result);
void get_cpu_usage(int pid, char *result);
void get_ports(int pid, char *result);
void get_fds(int pid, char *result);

#endif