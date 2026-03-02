#include "network.h"

// SERVER SIDE: Initialize socket, bind to port, and start listening
int setup_server(int port)
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // 1. Create socket file descriptor (IPv4, TCP)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        return -1;
    }

    // 2. Forcefully attaching socket to the port (prevents "Address already in use" errors)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
    address.sin_port = htons(port);       // Host-to-Network Short (Endianness conversion)

    // 3. Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        return -1;
    }

    // 4. Start listening for incoming connections
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        return -1;
    }

    return server_fd;
}

// CLIENT SIDE: Create socket and connect to the server's IP/Port
int connect_to_server(const char *ip, int port)
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    return sock;
}