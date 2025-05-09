#include "Client.hpp"

Client::Client(int id){
    _clientID = id;
    _nickName = "";
    _userName = "";
    _channel = "";
    _lastTriedNick = "";
    _hasPassword = false;

    std::cout << "Client created with id: " << id << std::endl;
}

Client::~Client(){}

Client &Client::operator=(const Client &src) {
    _clientID = src._clientID;
    _nickName = src._nickName;
    _userName = src._userName;
    _channel = src._channel;
    _lastTriedNick = src._lastTriedNick;
    _cmd = src._cmd;
    return *this;
}

Client::Client(const Client &src) {
    *this = src;
}

int Client::getId()  {
    return _clientID;
}

std::stringstream &Client::getBuffer()  {
	return (_buffer);
}

std::string &Client::getNickName()  {
    return (_nickName);
}

std::string &Client::getUserName()  {
    return (_userName);
}

std::vector<std::string> Client::getCommand(void)  {
   return (_cmd);
}

std::string &Client::getChannel()  {
    return _channel;
}

std::string &Client::getLastNick() {
    return _lastTriedNick;
}

std::string Client::getClientPrefix()  {
    std::string clientPrefix = getNickName() + "!" + \
        getUserName() + "@" + Manager::hostName;
    return clientPrefix;
}

void Client::setNickName(std::string nickName) {
    _nickName = nickName;
}


void Client::setUserName(std::string userName) {
    _userName = userName;
}


void Client::setHostName(std::string hostname) {
    _hostname = hostname;
}

void Client::setLastNick(std::string lastNick) {
    _lastTriedNick = lastNick;
}

void Client::setCommand(std::string cmd) {
    _cmd.clear();
    if (cmd[0] == '/')
        cmd.erase(0, 1);
    std::string holder;
    if ((int)cmd.find(" ") > 0) {
        holder = Parser::toUpper(((cmd.substr(0, cmd.find(" ")))));
        _cmd.push_back(holder);
        _cmd.push_back(cmd.substr(cmd.find(" ") + 1, cmd.size()));
    }
    else {
        holder = Parser::toUpper(cmd.substr(0, cmd.size()));
        _cmd.push_back(holder);
    }
    // std::cout << "cmd[0] = " << _cmd[0] << "; cmd[1] = " << _cmd[1] << std::endl;
    // for (unsigned long i = 0; i < _cmd.size(); i++) {
    //     std::cout << _cmd[i] << std::endl;
    // }
}

void Client::setregularCommand(std::string cmd) {
    _cmd.clear();
    if (cmd[0] == '/')
        cmd.erase(0, 1);

    _cmd.push_back(cmd);
}

void Client::setChannel(std::string channel) {
    _channel = channel;
}

bool Client::hasPassword() const { return _hasPassword; }
void Client::setHasPassword(bool value) { _hasPassword = value; }
