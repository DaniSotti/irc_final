#include "Channel.hpp"

// constructor: initializes a channel with the given ID
Channel::Channel(std::string id){
    _channelId = id; // set the channel ID
    _key = "";       // initialize the channel key
    _topic = "";     // initialize the channel topic
    _ModeI = 0;      // invite-only mode (0 = off, 1 = on)
    _ModeT = 0;      // topic protection mode (0 = off, 1 = on)
    _limit = 0;      // client limit (0 = no limit)
    std::cout << "Channel created with id: " << id << std::endl;
    Manager::addChannelName(id); // register the channel name in the manager
}

// assignment operator: copies data from another channel
Channel &Channel::operator=(const Channel &src) {
    _channelId = src._channelId;
    _key = src._key;
    _topic = src._topic;
    _ModeI = src._ModeI;
    _ModeT = src._ModeT;
    _limit = src._limit;
    return (*this);
}

// destructor: logs when the channel is destroyed
Channel::~Channel(){
}

// adds a client to the channel
void Channel::addClient(int newClient) {
    if (_limit && (int)_Clients.size() == _limit) // check if the channel is full
        return;
    _Clients.push_back(newClient); // add the client to the list
    removeInvited(newClient);      // remove the client from the invited list
    Manager::getClientByID(newClient)->setChannel(_channelId); // set the client's channel
    if (_ClientOperators.size() == 0) {
        addClientToOp(newClient); // make the first client an operator
    } else {
        updateList(); // update the client list
    }
}

// adds a client to the operator list
void Channel::addClientToOp(int newOp) {
    for (int i = 0; i < (int)_ClientOperators.size(); i++) {
        if (_ClientOperators[i] == newOp) {
            return; // client is already an operator
        }
    }
    _ClientOperators.push_back(newOp); // add the client to the operator list
    updateList(); // update the client list
}

// adds a client to the invited list
void Channel::addInvited(int newInvited) {
    for (int i = 0; i < (int)_invited.size(); i++) {
        if (_invited[i] == newInvited) {
            return; // client is already invited
        }
    }
    _invited.push_back(newInvited); // add the client to the invited list
}

// removes a client from the channel
void Channel::removeClient(int id) {
    for (int i = 0; i < (int)_Clients.size(); i++) {
        if (_Clients[i] == id) {
            _Clients.erase(_Clients.begin() + i); // remove the client from the list
        }
    }
    Manager::getClientByID(id)->setChannel(""); // clear the client's channel
    if (IsOp(id)) {
        removeOp(id); // remove the client from the operator list
    } else {
        updateList(); // update the client list
    }
}

// removes a client from the operator list
void Channel::removeOp(int id) {
    for (int i = 0; i < (int)_ClientOperators.size(); i++) {
        if (_ClientOperators[i] == id) {
            _ClientOperators.erase(_ClientOperators.begin() + i); // remove the client
        }
    }
    if ((int)_ClientOperators.size() == 0 && (int)_Clients.size()) {
        addClientToOp(_Clients[0]); // promote the first client to operator
    } else {
        updateList(); // update the client list
    }
}

// removes a client from the invited list
void Channel::removeInvited(int id) {
    for (int i = 0; i < (int)_invited.size(); i++) {
        if (_invited[i] == id) {
            _invited.erase(_invited.begin() + i); // remove the client
        }
    }
}

// sends a message to all clients in the channel
void Channel::channelMessage(std::string msg) {
    for (int i = 0; i < (int)_Clients.size(); i++) {
        if (send(_Clients[i], msg.c_str(), msg.size(), 0) == -1)
            std::cout << "error sending" << std::endl;
    }
}

// sends a message from one client to all other clients in the channel
void Channel::clientMessage(int client, const char *msg) {
    for (int i = 0; i < (int)_Clients.size(); i++) {
        if (_Clients[i] != client && Manager::sendIrcMessage((int)_Clients[i], msg) == -1) {
            std::cout << "error sending" << std::endl;
        }
    }
}

// checks if a client is in the channel
bool Channel::checkClient(int clientId) const {
    for (int i = 0; i < (int)_Clients.size(); i++) {
        if (_Clients[i] == clientId)
            return (true);
    }
    return (false);
}

// getters for channel attributes
std::string &Channel::getChannelId() { return _channelId; }
std::string &Channel::getKey() { return _key; }
std::string &Channel::getTopic() { return _topic; }
int &Channel::getModeI() { return _ModeI; }
int &Channel::getModeT() { return _ModeT; }
int &Channel::getModeL() { return _limit; }

// returns a string representation of the channel's modes
std::string Channel::getChannelModes() {
    std::string modes;

    // invite-only mode
    if (_ModeI)
        modes += "+i ";
    else
        modes += "-i ";

    // topic protection mode
    if (_ModeT)
        modes += "+t ";
    else
        modes += "-t ";

    // key-protected mode
    if (!_key.empty())
        modes += "+k ";
    else
        modes += "-k ";

    // client limit mode
    if (!_limit)
        modes += "-l";
    else
        modes += "+l";

    return (modes);
}

// returns the number of clients in the channel
int Channel::howManyClients() {
    return (_Clients.size());
}

// checks if a client is an operator
int Channel::IsOp(int id) {
    for (int i = 0; i < (int)_ClientOperators.size(); i++) {
        if (_ClientOperators[i] == id) {
            return (1);
        }
    }
    return 0;
}

// checks if a client is invited
int Channel::IsInvited(int id) {
    for (int i = 0; i < (int)_invited.size(); i++) {
        if (_invited[i] == id) {
            return (1);
        }
    }
    return 0;
}

// getters for client and operator lists
std::vector<int> &Channel::getClients() { return _Clients; }
std::vector<int> &Channel::getOps() { return _ClientOperators; }

// sets invite-only mode
void Channel::setModeI(int flag) { _ModeI = flag ? 1 : 0; }

// sets topic protection mode
void Channel::setModeT(int flag) { _ModeT = flag ? 1 : 0; }

// sets the client limit
void Channel::setModeL(int flag) { _limit = flag; }

// returns a list of client nicknames in the channel
std::vector<std::string> Channel::getNamesList() {
    std::vector<std::string> namesList;

    for (int i = 0; i < (int)_Clients.size(); i++) {
        int clientId = _Clients[i];
        std::string nickname;
        if (IsOp(clientId))
            nickname = "@" + Manager::getNickbyID(clientId); // operator prefix
        else
            nickname = "%" + Manager::getNickbyID(clientId); // regular client prefix
        namesList.push_back(nickname);
    }

    return namesList;
}

// updates the client list for all clients in the channel
void Channel::updateList() {
    for (int i = 0; i < (int)_Clients.size(); i++) {
        Manager::sendNamesList(_channelId, *Manager::getClientByID(_Clients[i]));
    }
}
