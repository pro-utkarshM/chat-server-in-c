# Chat Server in C

## Description

This project is a simple chat server implemented in C. It allows multiple clients to connect and communicate with each other through a central server. The server manages client connections and broadcasts messages to all connected clients.

## Features

- Supports multiple client connections
- Broadcasts messages to all connected clients
- Simple command-line interface
- Efficient handling of client connections

## Installation

To compile and run the chat server, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/pro-utkarshM/chat-server-in-c.git
    cd chat-server-in-c
    ```

2. Compile the server:
    ```sh
    gcc -o chat_server chat_server.c
    ```

3. Compile the client:
    ```sh
    gcc -o chat_client chat_client.c
    ```

## Usage

### Running the Server

To start the chat server, run:
```sh
./chat_server <port>
```
Replace `<port>` with the port number you want the server to listen on.

### Running the Client

To connect to the server as a client, run:
```sh
./chat_client <server_ip> <port>
```
Replace `<server_ip>` with the IP address of the server and `<port>` with the port number the server is listening on.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
