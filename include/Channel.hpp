#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "Server.hpp"

class Client;

class Channel {
    private:
        std::string _channelId; // channel name
        std::vector<int> _ClientOperators; // list of operator client IDs
        std::vector<int> _Clients; // list of all client IDs in the channel
        std::vector<int> _invited; // list of invited client IDs

        // channel modes and attributes
        std::string _key; // channel password
        std::string _topic; // channel topic
        int _ModeI; // invite-only mode (1 = on, 0 = off)
        int _ModeT; // topic restriction mode (1 = ops only, 0 = everyone)
        int _limit; // maximum number of clients allowed in the channel

    public:
        Channel() {;} // default constructor
        Channel(std::string id); // constructor with channel ID
        Channel &operator=(const Channel &src); // assignment operator
        ~Channel(); // destructor

        // methods to add clients
        void addClient(int newClient); // adds a client to the channel
        void addClientToOp(int newOp); // adds a client to the operator list
        void addInvited(int newInvited); // adds a client to the invited list

        // methods to remove clients
        void removeClient(int id); // removes a client from the channel
        void removeOp(int id); // removes a client from the operator list
        void removeInvited(int id); // removes a client from the invited list

        // updates the list of users in the channel for all clients
        void updateList();

        // methods to send messages
        void channelMessage(std::string msg); // sends a message to all clients in the channel
        void clientMessage(int client, const char *msg); // sends a message from one client to others

        // getters for channel attributes
        std::string &getChannelId(); // gets the channel ID
        std::string &getKey(); // gets the channel password
        std::string &getTopic(); // gets the channel topic
        int &getModeI(); // gets the invite-only mode
        int &getModeT(); // gets the topic restriction mode
        int &getModeL(); // gets the client limit
        std::string getChannelModes(); // gets a string representation of the channel modes
        int IsOp(int i); // checks if a client is an operator
        int IsInvited(int i); // checks if a client is invited
        std::vector<int> &getClients(); // gets the list of all clients
        std::vector<int> &getOps(); // gets the list of operator clients
        int howManyClients(); // gets the number of clients in the channel

        // setters for channel attributes
        void setKey(std::string pass) {_key = pass;} // sets the channel password
        void setTopic(std::string topic) {_topic = topic;} // sets the channel topic
        void setModeI(int flag); // sets the invite-only mode
        void setModeT(int flag); // sets the topic restriction mode
        void setModeL(int flag); // sets the client limit

        // utility methods
        bool checkClient(int clientId) const; // checks if a client is in the channel
        std::vector<std::string> getNamesList(); // gets a list of client nicknames in the channel
};

#endif