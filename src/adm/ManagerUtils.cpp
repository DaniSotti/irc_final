#include "Manager.hpp"

void Manager::createMap(void) {
    _actionMap["JOIN"] = joinAction;
    _actionMap["NICK"] = nickAction;
    _actionMap["INVITE"] = inviteAction;
    _actionMap["KICK"] = kickAction;
    _actionMap["TOPIC"] = topicAction;
    _actionMap["MODE"] = modeAction;
	_actionMap["PRIVMSG"] = privmsgAction;
    _actionMap["WHO"] = whoAction;
    _actionMap["USER"] = userAction;
    _actionMap["PASS"] = passAction;
    _actionMap["CAP"] = capAction;
    _actionMap["QUIT"] = quitAction;
}

std::string Manager::formatMessage(Client &client) {
	return (":" + client.getNickName() + "!" + client.getUserName() + "@" + hostName);
}

std::string Manager::formatMessage(Client &client, std::string message)
{
	return (":" + hostName + " " + message + " " + client.getNickName());
}

int	Manager::sendIrcMessage(int clientId, std::string message)
{
	message = message + "\r\n";
	std::cout << "Sending message: " << message;
	if (send(clientId, message.c_str(), message.length(), 0) == -1)
		exit(4);
	return 0;
}

void Manager::runActions(Client &client) {
    const std::vector<std::string> &cmds = client.getCommand();
    if (cmds.empty())
    {
        return; // ou envie uma mensagem de erro adequada
    }
    else
    {
        std::string cmd = cmds[0];
        if (!client.hasPassword() && cmd != "PASS" && cmd != "CAP" && cmd != "QUIT") {
            sendIrcMessage(client.getId(), formatMessage(client, "464") + " :You must provide the correct password");
            return;
        }


        std::string action = cmd;
        std::map<std::string, eventFunction>::iterator it = _actionMap.find(action);
        if (it != _actionMap.end()) {
            it->second(client);
        } else {
            sendIrcMessage(client.getId(), formatMessage(client, UNKNOWNCOMMAND) + " :Unknown command");
        }
    }
}


// void Manager::runActions(Client &client){
//     (void) client;
//     std::string cmd = client.getCommand()[0];
//     if (!client.hasPassword() && cmd != "PASS" && cmd != "CAP" && cmd != "QUIT") {
// 		sendIrcMessage(client.getId(), formatMessage(client, "464") + " :You must provide the correct password");
// 		return;
// 	}
//     std::string action = cmd;
//     std::map<std::string, eventFunction>::iterator it = _actionMap.find(action);
//     if (_actionMap.find(action) != _actionMap.end()){
//         it->second(client);
//     }
//     else{
//         sendIrcMessage(client.getId(), formatMessage(client, UNKNOWNCOMMAND) + " :Unknown command");
//     }
// }