#include "Manager.hpp"

void Manager::privmsgAction(Client &client)
{
    std::vector<std::string> command = client.getCommand();
    if (command.size() < 2 || (int)command[1].find(":") < 0) {
		sendIrcMessage(client.getId(), formatMessage(client, NEEDMOREPARAMS) + " COMMAND ERROR :Not enough parameters");
		return;
	}
    std::string targetName = command[1].substr(0, command[1].find(" "));
    std::string msg = command[1].substr(command[1].find(":") + 1, command[1].size());

    // Check if the target is a valid channel or user
    // if (target[0] != '#' && !Manager::isUserInChannel(target, client.getChannel())) {
    //     sendIrcMessage(client.getId(), formatMessage(client, ERR_NOSUCHNICK) + " " + target + " :No such nick/channel");
    //     return;
    // }

    if (targetName[0] == '#') {
        std::string channelName = targetName;
        std::string formatmsg = formatMessage(client) + " PRIVMSG " + targetName + " :" + msg;
        if (_channels[channelName].checkClient(client.getId()))
            _channels[channelName].clientMessage(client.getId(), formatmsg.c_str());
        else
            sendIrcMessage(client.getId(), formatMessage(client, NOTONCHANNEL) + " " + targetName + " :Not in channel");
    } 
    else {
        int targetId = Manager::getIDbyNick(targetName);
        if (targetId != -1) {
            std::string ircMessage = formatMessage(client) + " PRIVMSG " + targetName + " :" + msg;
            sendIrcMessage(targetId, ircMessage);
        } else {
            sendIrcMessage(client.getId(), formatMessage(client, NOSUCHNICK) + " " + targetName + " :No such nick");
        }
    }
}