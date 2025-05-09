#include "Manager.hpp"

void Manager::topicAction(Client &client) {
    if (Parser::topicParse(client)) {
        std::vector<std::string> command = client.getCommand();
        std::string topic;
        std::string channelName;
        if ((int)command[1].find(":") > 0) {
            topic = command[1].substr(command[1].find(":") + 1, command[1].size());
            channelName = command[1].substr(0, command[1].find(" "));
        }
        else {
            channelName = command[1];
        }
        if ((int)command[1].find(":") < 0 && _channels.find(channelName)->second.getTopic().empty()) {
            sendIrcMessage(client.getId(), formatMessage(client, NOTOPIC) + " " + channelName + " :No topic is set");
        }
        else if ((int)command[1].find(":") < 0) {
            sendIrcMessage(client.getId(), formatMessage(client, TOPIC_CHANNEL) + " " + channelName + " :" + _channels.find(channelName)->second.getTopic());
        }
        else if (_channels.find(channelName)->second.getModeT() && !_channels.find(channelName)->second.IsOp(client.getId())) {
            sendIrcMessage(client.getId(), formatMessage(client, NOTCHANOP) + " " + channelName + " :You're not channel operator");
        }
        else {
            _channels.find(channelName)->second.setTopic(topic);
            sendIrcMessage(client.getId(), formatMessage(client, TOPIC_CHANNEL) + " " + channelName + " :" + _channels.find(channelName)->second.getTopic());
            _channels.find(channelName)->second.channelMessage(formatMessage(client, TOPIC_CHANNEL) + " " + channelName + " :" + topic + "\r\n");
        }
    }
}