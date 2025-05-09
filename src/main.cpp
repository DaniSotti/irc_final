#include "Server.hpp"

void printUsage() {
    std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
    std::cout << "  <port>     : Port number to run the IRC server on (e.g., 6667)" << std::endl;
    std::cout << "  <password> : Password required for clients to connect" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        printUsage();
        return EXIT_FAILURE;
    }

    Server socket_serv(argv[1], argv[2]);
    return EXIT_SUCCESS;
}