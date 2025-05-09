#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "HexchatMsg.hpp"
#include "Manager.hpp"
#include "Parser.hpp"
#include "Client.hpp"

// maximum number of connections waiting to be accepted
#define BACKLOG 10

// port range definitions
#define MAX_PORT 65535 // maximum port number for TCP/UDP
#define MIN_PORT 1023  // minimum port number (ports below 1023 are reserved)

// buffer size for handling client data
#define BUFFER_SIZE 1000

// maximum length for names (e.g., channel or client names)
#define MAXNAME 20

// forward declaration of FileManage class
class FileManage;

// server class to manage the IRC server
class Server {
    private:
        std::string _port; // port number for the server
        int _socketFd;     // listening socket file descriptor
        std::vector<struct pollfd> pollfds; // vector to store pollfd structures for monitoring file descriptors
        int _maxFd;        // maximum file descriptor value

        // private methods for internal server operations
        void parsePortPass(); // validates the port and password
        void initSocket();    // initializes the server socket
        struct addrinfo *socketAddress(); // creates and returns the socket address structure
        void startMainLoop(); // starts the main server loop
        void getData();       // handles incoming data from clients
        void acceptedConnection(int newFd); // handles a new client connection
        void handleData(int i); // processes data from a specific client
        void handleMessage(int i); // processes a message from a client

    public:
        // constructor and destructor
        Server(std::string port, std::string password); // initializes the server with a port and password
        ~Server(); // cleans up resources when the server is destroyed

        // public utility methods
        void parsePortPass(std::string port, std::string password); // validates the port and password
        std::vector<std::string> divideString(const std::string &str, char delim); // splits a string into tokens
};

#endif

/*
+--------------------+
| Start Server       |
| (Constructor)      |
+--------------------+
          |
          v
+--------------------+
| Parse Port &       |
| Password           |
+--------------------+
          |
          v
+--------------------+
| Initialize Socket  |
| (Bind & Listen)    |
+--------------------+
          |
          v
+--------------------+
| Start Main Loop    |
| (poll())           |
+--------------------+
          |
          v
+----------------------------+
| Check for Events on FDs    |
| (New Connection or Data)   |
+----------------------------+
                        |
                +------+------+
                |             |
                v             v
+--------------------+   +--------------------+
| Accept New Client  |   | Handle Data from   |
| Connection         |   | Existing Client    |
+--------------------+   +--------------------+
          |                     |
          v                     v
+--------------------+   +--------------------+
| Add Client to      |   | Parse & Process    |
| Poll List          |   | Client Message     |
+--------------------+   +--------------------+
          |                     |
          v                     v
+--------------------+   +--------------------+
| Continue Main Loop |   | Disconnect Client  |
+--------------------+   +--------------------+
          |
          v
+--------------------+
| Stop Server        |
| (Destructor)       |
+--------------------+
*/