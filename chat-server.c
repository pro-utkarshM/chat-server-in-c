#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX_CLIENTS 100
#define MAX_ROOMS 10

struct client_info {
    int fd;
    char *username;
    char *room;
};

struct room_info {
    char *name;
    int num_clients;
    int clients[MAX_CLIENTS];
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    struct pollfd fds[MAX_CLIENTS + 1];
    struct client_info clients[MAX_CLIENTS];
    struct room_info rooms[MAX_ROOMS];
    int nfds = 1;
    int nrooms = 0;

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        int ret = poll(fds, nfds, -1);
        if (ret < 0) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
            struct sockaddr_in client_address;
            socklen_t client_addrlen = sizeof(client_address);
            int new_socket;
            if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d\n", new_socket);

            if (nfds < MAX_CLIENTS + 1) {
                fds[nfds].fd = new_socket;
                fds[nfds].events = POLLIN;
                clients[nfds - 1].fd = new_socket;
                clients[nfds - 1].username = NULL;
                clients[nfds - 1].room = NULL;
                nfds++;
            } else {
                printf("Max clients reached. Connection rejected.\n");
                close(new_socket);
            }
        }

        for (int i = 1; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                char buffer[1024] = {0};
                int valread = read(fds[i].fd, buffer, 1024);

                if (valread == 0) {
                    printf("Client disconnected, socket fd is %d\n", fds[i].fd);
                    close(fds[i].fd);
                    free(clients[i - 1].username);
                    for (int j = i; j < nfds - 1; j++) {
                        fds[j] = fds[j + 1];
                        clients[j - 1] = clients[j];
                    }
                    nfds--;
                    i--;
                } else {
                    if (clients[i - 1].username == NULL) {
                        clients[i - 1].username = strdup(buffer);
                        char welcome_msg[100];
                        sprintf(welcome_msg, "Welcome, %s!\n", clients[i-1].username);
                        send(fds[i].fd, welcome_msg, strlen(welcome_msg), 0);
                    } else {
                        if (strncmp(buffer, "/create", 7) == 0) {
                            char *room_name = strtok(buffer + 8, " \n");
                            if (nrooms < MAX_ROOMS) {
                                rooms[nrooms].name = strdup(room_name);
                                rooms[nrooms].num_clients = 0;
                                nrooms++;
                                char success_msg[100];
                                sprintf(success_msg, "Room %s created.\n", room_name);
                                send(fds[i].fd, success_msg, strlen(success_msg), 0);
                            } else {
                                char error_msg[100] = "Max rooms reached.\n";
                                send(fds[i].fd, error_msg, strlen(error_msg), 0);
                            }
                        } else if (strncmp(buffer, "/join", 5) == 0) {
                            char *room_name = strtok(buffer + 6, " \n");
                            int room_idx = -1;
                            for (int j = 0; j < nrooms; j++) {
                                if (strcmp(rooms[j].name, room_name) == 0) {
                                    room_idx = j;
                                    break;
                                }
                            }
                            if (room_idx != -1) {
                                clients[i - 1].room = rooms[room_idx].name;
                                rooms[room_idx].clients[rooms[room_idx].num_clients++] = fds[i].fd;
                                char success_msg[100];
                                sprintf(success_msg, "Joined room %s.\n", room_name);
                                send(fds[i].fd, success_msg, strlen(success_msg), 0);
                            } else {
                                char error_msg[100];
                                sprintf(error_msg, "Room %s not found.\n", room_name);
                                send(fds[i].fd, error_msg, strlen(error_msg), 0);
                            }
                        } else if (strcmp(buffer, "/leave\n") == 0) {
                            if (clients[i - 1].room != NULL) {
                                char *room_name = clients[i - 1].room;
                                clients[i - 1].room = NULL;
                                for (int j = 0; j < nrooms; j++) {
                                    if (strcmp(rooms[j].name, room_name) == 0) {
                                        for (int k = 0; k < rooms[j].num_clients; k++) {
                                            if (rooms[j].clients[k] == fds[i].fd) {
                                                for (int l = k; l < rooms[j].num_clients - 1; l++) {
                                                    rooms[j].clients[l] = rooms[j].clients[l + 1];
                                                }
                                                rooms[j].num_clients--;
                                                break;
                                            }
                                        }
                                        break;
                                    }
                                }
                                char success_msg[100];
                                sprintf(success_msg, "Left room %s.\n", room_name);
                                send(fds[i].fd, success_msg, strlen(success_msg), 0);
                            }
                        } else if (strcmp(buffer, "/list\n") == 0) {
                            if (clients[i - 1].room != NULL) {
                                char user_list[1024] = "Users in this room:\n";
                                for (int j = 0; j < nrooms; j++) {
                                    if (strcmp(rooms[j].name, clients[i - 1].room) == 0) {
                                        for (int k = 0; k < rooms[j].num_clients; k++) {
                                            for (int l = 0; l < nfds - 1; l++) {
                                                if (clients[l].fd == rooms[j].clients[k]) {
                                                    strcat(user_list, clients[l].username);
                                                    strcat(user_list, "\n");
                                                    break;
                                                }
                                            }
                                        }
                                        break;
                                    }
                                }
                                send(fds[i].fd, user_list, strlen(user_list), 0);
                            }

                        } else {
                            if (clients[i - 1].room != NULL) {
                                char message[1124];
                                sprintf(message, "[%s] %s: %s", clients[i - 1].room, clients[i - 1].username, buffer);
                                for (int j = 0; j < nrooms; j++) {
                                    if (strcmp(rooms[j].name, clients[i - 1].room) == 0) {
                                        for (int k = 0; k < rooms[j].num_clients; k++) {
                                            if (rooms[j].clients[k] != fds[i].fd) {
                                                send(rooms[j].clients[k], message, strlen(message), 0);
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}