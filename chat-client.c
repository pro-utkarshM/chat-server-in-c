#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connected to server\n");

    char username[50];
    printf("Enter your username: ");
    fgets(username, 50, stdin);
    send(sock, username, strlen(username), 0);

    struct pollfd fds[2] = {
        {
            0,
            POLLIN,
            0
        },
        {
            sock,
            POLLIN,
            0
        }
    };

    while (1) {
        char buffer[1024] = {0};

        poll(fds, 2, -1);

        if (fds[0].revents & POLLIN) {
            read(0, buffer, 1024);
            send(sock, buffer, strlen(buffer), 0);
        } else if (fds[1].revents & POLLIN) {
            int valread = read(sock, buffer, 1024);
            if (valread == 0) {
                printf("Server disconnected\n");
                break;
            }
            printf("%s", buffer);
        }
    }

    return 0;
}
