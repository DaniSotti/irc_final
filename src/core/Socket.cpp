#include "Server.hpp"

// constructor: initializes the socket with the given port and password
Server::Server(std::string port, std::string password) : _port(port) {
    std::cout << "Socket Constructor called" << std::endl;
    std::cout << "User pass> " << password << std::endl;

    Manager::setPassword(password); // set the server password
    parsePortPass(port, password);  // validate port and password
    initSocket();                   // initialize the server socket
    startMainLoop();                // start the main event loop
}

Server::~Server() {
    std::cout << "Socket destroyed" << std::endl;
}

void Server::parsePortPass(std::string port, std::string password) {
    if (port.length() <= 0 || atoi(port.c_str()) <= MIN_PORT || atoi(port.c_str()) > MAX_PORT) {
        std::cout << "Invalid port" << std::endl;
        exit(1);
    }
    if (password.length() == 0) {
        std::cout << "Invalid Password" << std::endl;
        exit(1);
    }
}

// creates and returns the socket address structure
struct addrinfo *Server::socketAddress() {
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof(hints)); // clear the hints structure
    hints.ai_family = AF_UNSPEC;     // allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // use TCP
    hints.ai_flags = AI_PASSIVE;     // use the local IP address

    // get address info for the server
    if ((status = getaddrinfo(NULL, _port.c_str(), &hints, &servinfo)) != 0) {
        std::cout << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        exit(1);
    }
    return servinfo;
}

// initializes the server socket, binds it, and starts listening
void Server::initSocket() {
    struct addrinfo *server_socket, *p;
    int check_returns = 0;
    int yes = 1;

    server_socket = socketAddress(); // get the server address info
    for (p = server_socket; p != NULL; p = p->ai_next) {
        _socketFd = socket(server_socket->ai_family, server_socket->ai_socktype, server_socket->ai_protocol);
        if (_socketFd < 0) {
            std::cout << "couldnt open the socket" << std::endl;
            continue;
        }
        setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // allow address reuse
        check_returns = bind(_socketFd, server_socket->ai_addr, server_socket->ai_addrlen); // bind the socket
        if (check_returns < 0) {
            std::cout << "couldnt bind the socket" << std::endl;
            close(_socketFd);
        }
        break;
    }
    if (p == NULL) {
        std::cout << "Unable to bind" << std::endl;
        exit(1);
    }
    freeaddrinfo(server_socket); // free the address info structure
    check_returns = listen(_socketFd, BACKLOG); // start listening for connections
    if (check_returns < 0) {
        std::cout << "couldnt listen" << std::endl;
        exit(1);
    }
    std::cout << _socketFd << std::endl; // log the socket file descriptor
}

// main event loop: handles incoming connections and client data using poll()
void Server::startMainLoop() {
    Manager::createMap(); // initialize the manager

    // vector to store pollfd structures for monitoring file descriptors
    std::vector<struct pollfd> pollfds;

    // add the listening socket to the pollfds vector
    struct pollfd listenFd;
    listenFd.fd = _socketFd;
    listenFd.events = POLLIN; // monitor for incoming connections
    pollfds.push_back(listenFd);

    while (1) {
        // wait for events on the monitored file descriptors
        int pollCount = poll(pollfds.data(), pollfds.size(), -1);
        if (pollCount == -1) {
            std::cerr << "Error polling()" << std::endl;
            exit(4);
        }

        // iterate through the pollfds vector to handle events
        for (size_t i = 0; i < pollfds.size(); ++i) {
            if (pollfds[i].revents & POLLIN) { // check if there's data to read
                if (pollfds[i].fd == _socketFd) {
                    // accept a new connection
                    struct sockaddr_storage address;
                    socklen_t addrlen = sizeof(address);
                    int newFd = accept(_socketFd, (struct sockaddr *)&address, &addrlen);
                    if (newFd == -1) {
                        std::cerr << "Error accepting connection" << std::endl;
                    } else {
                        // add the new connection to the pollfds vector
                        struct pollfd clientFd;
                        clientFd.fd = newFd;
                        clientFd.events = POLLIN; // monitor for incoming data
                        pollfds.push_back(clientFd);
                        acceptedConnection(newFd); // handle the new connection
                    }
                } else {
                    // handle data from an existing client
                    handleData(pollfds[i].fd);

                    // if the client disconnected, remove it from the pollfds vector
                    if (!Manager::isClient(pollfds[i].fd)) {
                        close(pollfds[i].fd); // close the client socket
                        pollfds.erase(pollfds.begin() + i); // remove from the vector
                        --i; // adjust the index after removal
                    }
                }
            }
        }
    }
}
