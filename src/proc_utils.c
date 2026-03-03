#include "proc_utils.h"

// 1. Find PID by name using raw system calls
int get_pid_by_name(const char *proc_name)
{
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    if (!dir)
    {
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (!isdigit(entry->d_name[0]))
        {
            continue; // Skip non-numeric entries
        }

        char path[512], comm[256];
        snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);
        
        // Direct System Call: open
        int fd = open(path, O_RDONLY);
        if (fd != -1)
        {
            // Direct System Call: read
            ssize_t bytes = read(fd, comm, sizeof(comm) - 1);
            close(fd); // Direct System Call: close
            
            if (bytes > 0) 
            {
                comm[bytes] = '\0';
                comm[strcspn(comm, "\n")] = 0; // Remove newline
                
                if (strcmp(comm, proc_name) == 0) 
                {
                    closedir(dir);
                    return atoi(entry->d_name);
                }
            }
        }
    }
    closedir(dir);
    return -1;
}

// 2. Memory Usage (VmRSS) using system calls
void get_memory_usage(int pid, char *result)
{
    char path[512], buffer[4096];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    
    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        strcpy(result, "Error: Could not open status file.");
        return;
    }

    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        char *match = strstr(buffer, "VmRSS:");
        if (match)
        {
            char *end = strchr(match, '\n');
            if (end) *end = '\0';
            strcpy(result, match);
        }
        else
        {
            strcpy(result, "Error: Memory info (VmRSS) not found.");
        }
    }
}

// 3. CPU Usage using system calls
void get_cpu_usage(int pid, char *result)
{
    char path[512], buffer[1024];
    unsigned long utime, stime;
    long total_time1, total_time2;

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        strcpy(result, "Error: Could not read CPU info.");
        return;
    }
    read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    // Use sscanf on the buffer since we aren't using fscanf
    sscanf(buffer, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &utime, &stime);
    total_time1 = utime + stime;

    usleep(100000); // 100ms sample

    fd = open(path, O_RDONLY);
    if (fd != -1)
    {
        read(fd, buffer, sizeof(buffer) - 1);
        close(fd);
        sscanf(buffer, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &utime, &stime);
        total_time2 = utime + stime;
        snprintf(result, MAX_BUF, "CPU Delta Ticks: %ld", (total_time2 - total_time1));

    }
}

// 4. Open FDs (Count directories in /proc/pid/fd)
void get_fds(int pid, char *result)
{
    char path[512];
    snprintf(path, sizeof(path), "/proc/%d/fd", pid);
    DIR *dir = opendir(path);
    int count = 0;
    if (!dir)
    {
        strcpy(result, "Error: Access denied.");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip . and ..
        if (entry->d_name[0] != '.') count++;
    }
    closedir(dir);
    snprintf(result, MAX_BUF, "Open File Descriptors: %d", count);
}

// 5. Ports (Scan for socket links)
void get_ports(int pid, char *result)
{
    char path[512], link_path[1024];
    snprintf(path, sizeof(path), "/proc/%d/fd", pid);

    DIR *dir = opendir(path);
    int found = 0;
    result[0] = '\0';

    if (!dir)
    {
        strcpy(result, "Error: Access denied.");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.') continue;

        char fd_path[1024];
        snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%s", pid, entry->d_name);
        
        // Direct System Call: readlink (checks where the FD points)
        ssize_t len = readlink(fd_path, link_path, sizeof(link_path) - 1);
        if (len != -1)
        {
            link_path[len] = '\0';
            if (strstr(link_path, "socket"))
            {
                strcat(result, "[Socket Found] ");
                found = 1;
            }
        }
    }
    closedir(dir);
    if (!found) strcpy(result, "No active socket ports.");
}