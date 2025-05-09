#include "Server.hpp"

/* iterate through successful connections to get data to read */
void Server::getData() {
    struct sockaddr_storage address; // struct to store socket address (supports IPv4, IPv6, etc.)
    socklen_t addrlen;               // size of the socket address structure
    int newFd;

    // iterate through the pollfds vector to check for events
    for (size_t i = 0; i < pollfds.size(); ++i) {
        if (pollfds[i].revents & POLLIN) { // check if there's data to read
            if (pollfds[i].fd == _socketFd) {
                // new connection detected
                addrlen = sizeof(address);
                newFd = accept(_socketFd, (struct sockaddr *)&address, &addrlen);
                if (newFd == -1) {
                    std::cout << "Error accepting connection" << std::endl;
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

/* get the hostname for each client */
void Server::acceptedConnection(int newFd) {
    struct sockaddr_storage peerAddress; // struct to store the client's address
    socklen_t peerAddrlen = sizeof(peerAddress);

    // get the client's address
    getpeername(newFd, (struct sockaddr *)&peerAddress, &peerAddrlen);

    char host[NI_MAXHOST]; // buffer to store the hostname

    // convert the address to a string
    std::string ipAddress = inet_ntoa(((struct sockaddr_in *)&peerAddress)->sin_addr);

    // get the hostname using the IP address
    struct hostent *hostEntry = gethostbyname(ipAddress.c_str());
    if (hostEntry) {
        strncpy(host, hostEntry->h_name, NI_MAXHOST);
        host[NI_MAXHOST - 1] = '\0'; // ensure null termination
        std::cout << "Connection accepted from " << host << std::endl;
        Manager::addClient(newFd); // add the client to the manager
    } else {
        std::cerr << "gethostbyname failed" << std::endl;
    }
}

/* handle data from a client by filling the buffer */
void Server::handleData(int fd) {
    int nbrBytes;          // number of bytes received
    char buffer[BUFFER_SIZE]; // buffer to store received data

    bzero(buffer, sizeof(buffer)); // clear the buffer

    // check if the client has quit
    if (!Manager::isClient(fd)) {
        close(fd);           // close the socket
        return;
    }

    // check for errors or client disconnection
    nbrBytes = recv(fd, &buffer, sizeof(buffer) - 1, 0);
    if (nbrBytes <= 0) {
        if (nbrBytes == 0) {
            // client disconnected
            std::cout << "Client with socket " << fd << " disconnected" << std::endl;
            Manager::removeClientFromChannels(*Manager::getClientByID(fd)); // remove client from channels
            Manager::removeClient(fd); // remove client from the manager
        } else {
            std::cout << "Random receive error" << std::endl;
        }
        close(fd); // close the socket
    } else {
        // append the received data to the client's buffer
        Manager::getClientBuffer(fd).str(Manager::getClientBuffer(fd).str() + buffer);
        int newLine = Manager::getClientBuffer(fd).str().find('\n'); // check for a newline
        while (newLine >= 0) {
            // process the message up to the newline
            std::string temp = Manager::getClientBuffer(fd).str().substr(newLine + 1, Manager::getClientBuffer(fd).str().size());
            Manager::getClientBuffer(fd).str(Manager::getClientBuffer(fd).str().substr(0, newLine + 1));
            std::cout << "[" << fd << "]" << Manager::getClientBuffer(fd).str();
            handleMessage(fd); // handle the message (e.g., commands)
            Manager::getClientBuffer(fd).str(temp); // update the buffer with remaining data
            newLine = Manager::getClientBuffer(fd).str().find('\n'); // check for another newline
        }
    }
}

/* handle a message from a client */
void Server::handleMessage(int fd) {
    // sanitize the buffer to remove unwanted characters
    std::string temp = Manager::getClientBuffer(fd).str();
    Manager::getClientBuffer(fd).str(Parser::sanitizeBuffer(temp));

    // get the client object
    std::vector<Client>::iterator iter = Manager::getClientByID(fd);
    Client &temporary = *Manager::getClientByID(fd);

    // set the client's command
    temporary.setCommand(Manager::getClientBuffer(fd).str());

    // check if the command is an action or a normal message
    if (Parser::isAction(temporary.getCommand()[0], fd) || (Manager::getClientByID(fd)->getChannel().size() && Manager::normalMsg(temporary))) {
        Manager::runActions(*iter); // execute the action
    } else {
        // if the client is not in a channel, send a message
        std::cout << "sending regular you're not in channel msg" << std::endl;
        send(fd, "You are not in a channel, please join a channel!\n", 49, 0);
    }
}
