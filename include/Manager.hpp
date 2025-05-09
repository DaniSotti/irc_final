#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// forward declarations for Socket, Client, and Channel classes
class Socket;
class Client;
class Channel;

// typedef for event functions (functions that handle client actions)
typedef void (*eventFunction)(Client &client);

// manager class to handle global server operations
class Manager {
    private:
        static std::string _password; // server password
        static std::vector<Client> _clients; // list of all connected clients
        static std::vector<std::string> _channelNames; // list of all channel names
        static std::map<std::string, Channel> _channels; // map of channel names to Channel objects
        static std::map<std::string, eventFunction> _actionMap; // map of commands to their corresponding event functions

    public:
        static std::string hostName; // server hostname

        // client management
        static void addClient(int id); // adds a new client by ID
        static void removeClient(int id); // removes a client by ID
        static std::vector<Client> &getClient(); // returns a reference to the list of clients
        static std::vector<Client>::iterator getClientByID(int i); // gets a client by their ID
        static std::stringstream &getClientBuffer(int i); // gets the buffer for a specific client
        static std::string getNickbyID(int id); // gets a client's nickname by their ID
        static int getIDbyNick(std::string nick); // gets a client's ID by their nickname
        static std::vector<int> getAllClientsIds(); // gets a list of all client IDs

        // channel management
        static std::vector<std::string> getChannelNames(); // gets a list of all channel names
        static void addChannelName(std::string name); // adds a new channel name

        // action map management
        static void createMap(void); // initializes the action map
        static std::map<std::string, eventFunction> &getActionMap(); // gets the action map

        // channel actions
        static void joinAction(Client &client); // handles the JOIN command
        static void kickAction(Client &client); // handles the KICK command
        static void inviteAction(Client &client); // handles the INVITE command
        static void topicAction(Client &client); // handles the TOPIC command
        static void modeAction(Client &client); // handles the MODE command

        // client actions
        static void nickAction(Client &client); // handles the NICK command
        static void whoAction(Client &client); // handles the WHO command
        static void userAction(Client &client); // handles the USER command
        static void privmsgAction(Client &client); // handles the PRIVMSG command
        static void passAction(Client &client); // handles the PASS command
        static void quitAction(Client &client); // handles the QUIT command
        static void capAction(Client &client); // handles the CAP command
        static void runActions(Client &client); // executes the appropriate action for a client command

        // message handling
        static int sendIrcMessage(int clientId, std::string message); // sends a message to a specific client
        static void sendWhoMessage(const std::vector<int> &list, Client &client, std::string const &channelName); // sends a WHO message
        static void sendNamesList(const std::string &channelName, Client &client); // sends the list of names in a channel
        static std::string formatMessage(Client &client); // formats a message from a client
        static std::string formatMessage(Client &client, std::string message); // formats a message with additional content

        // channel and client utilities
        static std::map<std::string, Channel> &getChannels(); // gets the map of channels
        static int normalMsg(Client &client); // handles normal messages
        static void removeClientFromChannels(Client &client); // removes a client from all channels

        // client validation
        static int isClient(int id); // checks if a client exists by ID

        // password management
        static std::string &getPassword(); // gets the server password
        static int setPassword(std::string password, Client &client); // sets the server password
        static void setPassword(std::string password); // sets the server password
};

#endif

/*
+--------------------+
| Initialize Manager |
| (Static Variables) |
+--------------------+
          |
          v
+--------------------+
| Add/Remove Clients |
| (Client Management)|
+--------------------+
          |
          v
+--------------------+
| Add/Remove Channels|
|(Channel Management)|
+--------------------+
          |
          v
+--------------------+
| Map Commands to    |
| Event Functions    |
+--------------------+
          |
          v
+--------------------+
| Execute Actions    |
| (JOIN, NICK, etc.) |
+--------------------+
          |
          v
+--------------------+
| Send Messages to   |
| Clients/Channels   |
+--------------------+
          |
          v
+--------------------+
| Validate Password  |
| (Optional)         |
+--------------------+
*/