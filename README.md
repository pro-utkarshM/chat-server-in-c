# Chat Server in C

## Description

This project is a simple chat server implemented in C. It allows multiple clients to connect and communicate with each other through a central server. The server manages client connections, chat rooms, and private messages.

## Features

- Supports multiple client connections
- Chat rooms
- Private messaging
- Simple command-line interface
- Efficient handling of client connections

## Installation

To compile and run the chat server, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/pro-utkarshM/chat-server-in-c.git
    cd chat-server-in-c
    ```

2. Compile the server and client:
    ```sh
    make
    ```

## Usage

### Running the Server

To start the chat server, run:
```sh
./chat-server <port>
```
Replace `<port>` with the port number you want the server to listen on.

### Running the Client

To connect to the server as a client, run:
```sh
./chat-client <server_ip> <port>
```
Replace `<server_ip>` with the IP address of the server and `<port>` with the port number the server is listening on.

## Commands

- `/create <room_name>`: Creates a new chat room.
- `/join <room_name>`: Joins an existing chat room.
- `/leave`: Leaves the current chat room.
- `/list`: Lists all users in the current chat room.
- `/msg <username> <message>`: Sends a private message to the specified user.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.