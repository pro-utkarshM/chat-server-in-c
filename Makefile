CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

all: chat-server chat-client

chat-server: chat-server.c
	$(CC) $(CFLAGS) -o chat-server chat-server.c

chat-client: chat-client.c
	$(CC) $(CFLAGS) -o chat-client chat-client.c

clean:
	rm -f chat-server chat-client
