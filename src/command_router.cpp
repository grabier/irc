/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_router.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppeckham <ppeckham@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:15:09 by sstoev            #+#    #+#             */
/*   Updated: 2025/10/06 16:41:31 by ppeckham         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "command_router.hpp"
#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"
#include <sys/socket.h>
#include <iostream>
#include <cctype>

CommandRouter::CommandRouter(Server& server) : _server(server) {
	initCommandHandlers();
}

CommandRouter::~CommandRouter(void) { }

void	CommandRouter::initCommandHandlers(void) {
	_commandHandlers["PASS"] = &CommandRouter::handlePASS;//ok
	_commandHandlers["CAP"] = &CommandRouter::handleCAP;
	_commandHandlers["NICK"] = &CommandRouter::handleNICK;//ok
	_commandHandlers["USER"] = &CommandRouter::handleUSER;//ok
	_commandHandlers["JOIN"] = &CommandRouter::handleJOIN;
	_commandHandlers["PART"] = &CommandRouter::handlePART;
	_commandHandlers["PRIVMSG"] = &CommandRouter::handlePRIVMSG;
	_commandHandlers["NOTICE"] = &CommandRouter::handleNOTICE;
	_commandHandlers["MODE"] = &CommandRouter::handleMODE;
	_commandHandlers["TOPIC"] = &CommandRouter::handleTOPIC;
	_commandHandlers["KICK"] = &CommandRouter::handleKICK;
	_commandHandlers["INVITE"] = &CommandRouter::handleINVITE;
	_commandHandlers["PING"] = &CommandRouter::handlePING;
	_commandHandlers["PONG"] = &CommandRouter::handlePONG;
	_commandHandlers["QUIT"] = &CommandRouter::handleQUIT;
}

CommandRouter::CommandResult	CommandRouter::processCommand(int client_fd, const Message& msg) {
	Client* client = _server.getClientByFd(client_fd);

	if (!client) {
		return (CMD_INVALID);
	}

	if (msg.getCommand().empty()) {
		return (CMD_INVALID);
	}

	std::string command = msg.getCommand();
	std::cout << "command : " <<  command << std::endl;
	if (command == "CAP")
		return (handleCAP(*client, msg));
		

	// Check authentication requirements
	if (!client->isAuthenticated() && requiresAuthentication(command)) {
		sendError(*client, "464", ":Password incorrect");
		return (std::cout << "!auth\n", CMD_ERROR);
	}

	// Check registration requirements
	if (!client->isRegistered() && requiresRegistration(command)) {
		sendError(*client, "451", ":You have not registered");
		
		return (CMD_ERROR);
	}

	// Dispatch to appropriate handler
	std::map<std::string, CommandHandler>::iterator it = _commandHandlers.find(command);
	if (it != _commandHandlers.end()) {
		return ((this->*(it->second))(*client, msg));
	}
	else {
		sendError(*client, "421", command + " :Unknown command");
		return (CMD_ERROR);
	}
}

bool	CommandRouter::requiresRegistration(const std::string& command) const {
	return (command != "PASS" && command != "NICK" && command != "USER" &&
			command != "PING" && command != "PONG" && command != "QUIT");
}

bool	CommandRouter::requiresAuthentication(const std::string& command) const {
	return (command != "PASS" && command != "PING" && command != "PONG" && command != "QUIT");
}


// ----------------------------- AUTH & REGISTRATION CMD HANDLERS -----------------------------
 
CommandRouter::CommandResult	CommandRouter::handleCAP(Client& client, const Message& msg){
	(void)msg;
	std::string reply = ": myserver CAP * LS :\r\n";
	send(client.get_sock_fd(), reply.c_str(), reply.size(), 0);
	return (CMD_OK);
}


CommandRouter::CommandResult	CommandRouter::handlePASS(Client& client, const Message& msg) {
	if (client.isRegistered()) {
		sendError(client, "462", ":You may not reregister");
		return (CMD_ERROR);
	}

	if (msg.getParamCount() < 1) {
		sendError(client, "461", "PASS :Not enough parameters");
		return (CMD_ERROR);
	}

	std::string password = msg.getParamAt(0);

	if (password == _server.getPassword()) {
		client.set_authenticated_status(true);
		return (CMD_OK);
	}
	else {
		sendError(client, "464", ":Password incorrect");
		return (std::cout << "!wrong pass\n",CMD_ERROR);
	}
}

CommandRouter::CommandResult	CommandRouter::handleNICK(Client& client, const Message& msg) {
	if (msg.getParamCount() < 1) {
		sendError(client, "431", ":No nickname given");
		return (CMD_ERROR);
	}

	std::string nickname = msg.getParamAt(0);

	// Validate nickname format
	if (!validateNickname(nickname)) {
		sendError(client, "432", nickname + " :Erroneous nickname");
		return (CMD_ERROR);
	}

	// Check if nickname is already in use
	Client* existing_client = _server.getClientByNick(nickname);
	if (existing_client && existing_client != &client) {
		sendError(client, "433", nickname + " :Nickname is already in use");
		return (CMD_ERROR);
	}

	// Set nickname using existing Client method
	std::string nick_copy = nickname;
	client.set_nick(nick_copy);

	// Send welcome message if fully registered
	if (client.isRegistered()) {
		sendResponse(client, ":server 001 " + nickname + " :Welcome to the IRC server");
		sendResponse(client, ":server 002 " + nickname + " :Your host is server");
		sendResponse(client, ":server 003 " + nickname + " :This server was created today");
		sendResponse(client, ":server 004 " + nickname + " server 1.0 io tkol");
	}

	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleUSER(Client& client, const Message& msg) {
	if (client.isRegistered()) {
		sendError(client, "462", ":You may not reregister");
		return (CMD_ERROR);
	}

	if (msg.getParamCount() < 4) {
		sendError(client, "461", "USER :Not enough parameters");
		return (CMD_ERROR);
	}

	std::string username = msg.getParamAt(0);
	std::string realname = msg.getParamAt(3);

	// Set user info using existing Client methods
	std::string user_copy = username;
	std::string real_copy = realname;
	client.set_user(user_copy);
	client.set_real_name(real_copy);

	// Send welcome if fully registered
	if (client.isRegistered()) {
		std::string nick = client.get_nick();
		sendResponse(client, ":server 001 " + nick + " :Welcome to the IRC server");
		sendResponse(client, ":server 002 " + nick + " :Your host is server");
		sendResponse(client, ":server 003 " + nick + " :This server was created today");
		sendResponse(client, ":server 004 " + nick + " server 1.0 io tkol");
	}

	return (CMD_OK);
}

// ----------------------------- CHANNEL OPERATION HANDLERS -----------------------------

CommandRouter::CommandResult	CommandRouter::handleJOIN(Client& client, const Message& msg) {
	if (msg.getParamCount() < 1) {
		sendError(client, "461", "JOIN :Not enough parameters");
		return CMD_ERROR;
	}

	std::string channelName = msg.getParamAt(0);
	std::string key = (msg.getParamCount() > 1) ? msg.getParamAt(1) : "";

	// Validate channel name
	if (!validateChannelName(channelName)) {
		sendError(client, "403", channelName + " :No such channel");
		return (CMD_ERROR);
	}

	// Get or create channel
	Channel* channel = _server.getChannelByName(channelName);
	if (!channel) {
		channel = _server.createChannel(channelName);
		// First user becomes operator
		//std::cout << "añadimos a " << client.get_nick() << " de operator\n";
		channel->addClient(client, key);
		channel->addOperator(client);
	}

	// Use existing Client method (which validates and calls Channel methods)
	client.addChannel(channel, key);

	// Check if join was successful
	if (client.isInChannel(channel)) {
		std::string test = ":" + client.get_nick() + "!" + client.get_user() + "@" + "localhost";
		// Send JOIN confirmation to user
		sendResponse(client, test + " JOIN " + channelName);
		
		// Send topic if exists
		if (!channel->getTopic().empty()) {
			sendResponse(client, ":server 332 " + client.get_nick() + " " + 
						channelName + " :" + channel->getTopic());
		}
		
		// Send user list
		std::string userList = formatChannelUserList(*channel);
		//std::cout << "userlist: "<< userList << std::endl;
		
		sendResponse(client, test +  " 353 " + client.get_nick() + " = " + 
					channelName + " :" + userList);
		sendResponse(client, test + " 366 " + client.get_nick() + " " + 
					channelName + " :End of /NAMES list.");
		
		// Notify other users in channel
		if (channel->getClientList().size() != 1 && channel->getClientList().size() > 0)
			channel->broadcastMessage(":" + client.get_nick() + " JOIN " + channelName);
		return (CMD_OK);
	}
	else {
		// Join failed - channel methods already handle the reason
		if (channel->isFull())
			sendError(client, "471", client.get_nick() + " " + channelName + ":Cannot join channel (+l)");
		else
			sendError(client, "473", channelName + " :Cannot join channel");
		return CMD_ERROR;
	}
}

CommandRouter::CommandResult	CommandRouter::handlePART(Client& client, const Message& msg) {
	if (msg.getParamCount() < 1) {
		sendError(client, "461", "PART :Not enough parameters");
		return (CMD_ERROR);
	}

	std::string channelName = msg.getParamAt(0);
	std::string reason = (msg.getParamCount() > 1) ? msg.getParamAt(1) : "";

	Channel* channel = _server.getChannelByName(channelName);
	if (!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return (CMD_ERROR);
	}

	if (!client.isInChannel(channel)) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return (CMD_ERROR);
	}

	// Notify channel before leaving
	std::string partMsg = ":" + client.get_nick() + " PART " + channelName;
	if (!reason.empty()) {
		partMsg += " :" + reason;
	}
	channel->broadcastMessage(partMsg);

	// Use existing Client method to leave channel
	client.removeChannel(channel);

	// Remove empty channel
	if (channel->getClientCount() == 0) {
		_server.removeChannel(channelName);
	}

	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleTOPIC(Client& client, const Message& msg) {
	if (msg.getParamCount() < 1) {
		sendError(client, "461", "TOPIC :Not enough parameters");
		return (CMD_ERROR);
	}

	std::string channelName = msg.getParamAt(0);
	Channel* channel = _server.getChannelByName(channelName);

	if (!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return (CMD_ERROR);
	}

	if (!client.isInChannel(channel)) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return (CMD_ERROR);
	}

	if (msg.getParamCount() == 1) {
		// Query topic
		if (channel->getTopic().empty()) {
			sendResponse(client, ":server 331 " + client.get_nick() + " " + channelName + " :No topic is set");
		}
		else {
			sendResponse(client, ":server 332 " + client.get_nick() + " " + channelName + " :" + channel->getTopic());
		}
		return (CMD_OK);
	}
	else {
		// Set topic
		std::string topic = msg.getParamAt(1);
		std::string topic_copy = topic;
		
		if (channel->setTopic(topic_copy, client)) {
			// Success - broadcast to channel
			channel->broadcastMessage(":" + client.get_nick() + " TOPIC " + channelName + " :" + topic);
			return (CMD_OK);
		} 
		else {
			sendError(client, "482", channelName + " :You're not channel operator");
			return (CMD_ERROR);
		}
	}
}


// ----------------------------- CHANNEL MODERATION HANDLERS -----------------------------

CommandRouter::CommandResult	CommandRouter::handleMODE(Client& client, const Message& msg) {
	if (msg.getParamCount() < 1) {
		sendResponse(client, "221 " + client.get_nick() + " +r");
		return (CMD_OK);
	}

	std::string target = msg.getParamAt(0);
	
	if (target[0] == '#') {
		// Channel mode
		Channel* channel = _server.getChannelByName(target);
		if (!channel) {
			sendError(client, "403", target + " :No such channel");
			return (CMD_ERROR);
		}

		if (!client.isInChannel(channel)) {
			sendError(client, "442", target + " :You're not on that channel");
			return (CMD_ERROR);
		}

		if (msg.getParamCount() == 1) {
			// Query channel modes
			sendResponse(client, ":server 324 " + client.get_nick() + " " + target + " +nt");
			return (CMD_OK);
		}

		std::string modeString = msg.getParamAt(1);
		std::string param = (msg.getParamCount() > 2) ? msg.getParamAt(2) : "";
		
		if (modeString.empty()) {
			return (CMD_ERROR);
		}

		bool adding = (modeString[0] == '+');
		if (!adding && modeString[0] != '-') {
			return (CMD_ERROR);
		}

		for (std::size_t i = 1; i < modeString.length(); ++i) {
			char mode = modeString[i];
			
			// For modes that affect other users (like 'o'), need target client
			Client* targetClient = &client;
			if (mode == 'o' && !param.empty()) {
				targetClient = _server.getClientByNick(param);
				if (!targetClient) {
					sendError(client, "401", param + " :No such nick/channel");
					continue ;
				}
			}

			bool success;
			if (adding) {
				success = channel->setMode(mode, param, client, *targetClient);
			}
			else {
				success = channel->removeMode(mode, client, *targetClient);
			}

			if (success) {
				// Broadcast mode change
				std::string modeChange = ":" + client.get_nick() + " MODE " + target + " " + 
										(adding ? "+" : "-") + mode;
				if (!param.empty()) {
					modeChange += " " + param;
				}
				channel->broadcastMessage(modeChange);
			} 
			else {
				if (!channel->isOperator(client))
					sendError(client, "482", target + " :You're not channel operator");
				else if (!targetClient->isInChannel(channel))
					sendError(client, "441", client.get_nick() + " " + targetClient->get_nick()
						+ channel->getName() + " :They're not in channel");
				return (CMD_ERROR);
			}
		}
		return (CMD_OK);
	}
	else {
		// User mode - not implemented for now
		sendError(client, "501", "MODE :User modes not implemented");
		return (CMD_ERROR);
	}
}

CommandRouter::CommandResult	CommandRouter::handleKICK(Client& client, const Message& msg) {
	if (msg.getParamCount() < 2) {
		sendError(client, "461", "KICK :Not enough parameters");
		return CMD_ERROR;
	}

	std::string channelName = msg.getParamAt(0);
	std::string targetNick = msg.getParamAt(1);
	std::string reason = (msg.getParamCount() > 2) ? msg.getParamAt(2) : client.get_nick();

	Channel* channel = _server.getChannelByName(channelName);
	if (!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return (CMD_ERROR);
	}

	if (!client.isInChannel(channel)) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return (CMD_ERROR);
	}

	Client* target = _server.getClientByNick(targetNick);
	if (!target) {
		sendError(client, "401", targetNick + " :No such nick/channel");
		return (CMD_ERROR);
	}

	if (!target->isInChannel(channel)) {
		sendError(client, "441", targetNick + " " + channelName + " :They aren't on that channel");
		return (CMD_ERROR);
	}

	// Use existing Channel method
	if (channel->kickClient(client, *target)) {
		// Success - Channel::kickClient already broadcasts
		// Send KICK message to the kicked user specifically
		sendResponse(*target, ":" + client.get_nick() + " KICK " + channelName + " " + targetNick + " :" + reason);
		return (CMD_OK);
	}
	else {
		sendError(client, "482", channelName + " :You're not channel operator");
		return (CMD_ERROR);
	}
}

CommandRouter::CommandResult	CommandRouter::handleINVITE(Client& client, const Message& msg) {
	if (msg.getParamCount() < 2) {
		sendError(client, "461", "INVITE :Not enough parameters");
		return (CMD_ERROR);
	}

	std::string targetNick = msg.getParamAt(0);
	std::string channelName = msg.getParamAt(1);

	Channel* channel = _server.getChannelByName(channelName);
	if (!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return (CMD_ERROR);
	}

	if (!client.isInChannel(channel)) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return (CMD_ERROR);
	}

	Client* target = _server.getClientByNick(targetNick);
	if (!target) {
		sendError(client, "401", targetNick + " :No such nick/channel");
		return (CMD_ERROR);
	}

	if (target->isInChannel(channel)) {
		sendError(client, "443", targetNick + " " + channelName + " :is already on channel");
		return (CMD_ERROR);
	}

	if (channel->isFull()) {
		sendError(client, "471", targetNick + " " + channelName + ":Cannot join channel (+l)");
		return (CMD_ERROR);
	}

	// Use existing Channel method
	if (channel->inviteClient(client, *target)) {
		// Send INVITE to target user
		sendResponse(*target, ":" + client.get_nick() + " INVITE " + targetNick + " " + channelName);
		target->addChannel(channel, "");
		channel->addClient(*target, "");
		// Confirm to inviter
		sendResponse(client, ":server 341 " + client.get_nick() + " " + targetNick + " " + channelName);
		return (CMD_OK);
	}
	else {
		sendError(client, "482", channelName + " :You're not channel operator");
		return (CMD_ERROR);
	}
}


// ----------------------------- CHANNEL MODERATION HANDLERS -----------------------------

CommandRouter::CommandResult	CommandRouter::handlePRIVMSG(Client& client, const Message& msg) {
	if (msg.getParamCount() < 2) {
		sendError(client, "461", "PRIVMSG :Not enough parameters");
		return CMD_ERROR;
	}

	std::string target = msg.getParamAt(0);
	std::string message = msg.getParamAt(1);

	if (target[0] == '#') {
		// Channel message
		Channel* channel = _server.getChannelByName(target);
		if (!channel) {
			sendError(client, "403", target + " :No such channel");
			return CMD_ERROR;
		}

		if (!client.isInChannel(channel)) {
			sendError(client, "442", target + " :You're not on that channel");
			return CMD_ERROR;
		}

		// Use existing Channel method to broadcast
		std::string fullMsg = ":" + client.get_nick() + " PRIVMSG " + target + " :" + message;
		channel->broadcastMessage(fullMsg);
		return (CMD_OK);
	}
	else {
		// Private message
		Client* targetClient = _server.getClientByNick(target);
		if (!targetClient) {
			sendError(client, "401", target + " :No such nick/channel");
			return (CMD_ERROR);
		}

		std::string fullMsg = ":" + client.get_nick() + " PRIVMSG " + target + " :" + message;
		sendResponse(*targetClient, fullMsg);
		return (CMD_OK);
	}
}

CommandRouter::CommandResult	CommandRouter::handleNOTICE(Client& client, const Message& msg) {
	// NOTICE is like PRIVMSG but should not generate error responses
	if (msg.getParamCount() < 2) {
		return (CMD_OK); // Silently ignore
	}

	std::string target = msg.getParamAt(0);
	std::string message = msg.getParamAt(1);

	if (target[0] == '#') {
		Channel* channel = _server.getChannelByName(target);
		if (channel && client.isInChannel(channel)) {
			std::string fullMsg = ":" + client.get_nick() + " NOTICE " + target + " :" + message;
			channel->broadcastMessage(fullMsg);
		}
	}
	else {
		Client* targetClient = _server.getClientByNick(target);
		if (targetClient) {
			std::string fullMsg = ":" + client.get_nick() + " NOTICE " + target + " :" + message;
			sendResponse(*targetClient, fullMsg);
		}
	}
	return (CMD_OK);
}


// ----------------------------- CHANNEL CONNECTION MANAGEMENT HANDLERS -----------------------------

CommandRouter::CommandResult	CommandRouter::handlePING(Client& client, const Message& msg) {
	if (msg.getParamCount() < 1) {
		sendError(client, "461", "PING :Not enough parameters");
		return (CMD_ERROR);
	}

	std::string server = msg.getParamAt(0);
	sendResponse(client, ":server PONG server :" + server);
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handlePONG(Client& client, const Message& msg) {
	// PONG is a response to PING - typically no response needed
	// Just acknowledge receipt (optional logging for debugging)
	(void)client; (void)msg;
	
	// PONG responses are usually silent - no server response needed
	// The client is confirming it's still alive
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleQUIT(Client& client, const Message& msg) {
	std::string reason = (msg.getParamCount() > 0) ? msg.getParamAt(0) : "Client quit";

	// Notify all channels the client is in
	std::list<Channel*> channels = client.getChannels();
	for (std::list<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		(*it)->broadcastMessage(":" + client.get_nick() + " QUIT :" + reason);
		client.removeChannel(*it);
	}

	// Signal that client should be disconnected
	return (CMD_DISCONNECT);
}


// ----------------------------- UTILITY METHODS -----------------------------

void			CommandRouter::sendResponse(Client& client, const std::string& response) {
	std::string formatted = response;

	if (formatted.length() < 2 || formatted.substr(formatted.length() - 2) != "\r\n") {
		formatted += "\r\n";
	}

	send(client.get_sock_fd(), formatted.c_str(), formatted.length(), MSG_NOSIGNAL);
}

void			CommandRouter::sendError(Client& client, const std::string& code, const std::string& message) {
	std::string nick = client.get_nick().empty() ? "*" : client.get_nick();
	std::string error_msg = ":server " + code + " " + nick + message + "\r\n";
	
	send(client.get_sock_fd(), error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
}

bool			CommandRouter::validateChannelName(const std::string& name) const {
	if (name.empty() || name[0] != '#') {
		return (false);
	}

	// Check for invalid characters
	for (std::size_t i = 1; i < name.length(); ++i) {
		char c = name[i];
		if (c == ' ' || c == ',' || c == '\r' || c == '\n' || c == '\0') {
			return (false);
		}
	}
	return (true);
}

bool			CommandRouter::validateNickname(const std::string& nick) const {
	if (nick.empty()) {
		return (false);
	}

	// First character must be letter or special char
	char first = nick[0];
	if (!std::isalpha(first) && first != '[' && first != ']' && 
		first != '\\' && first != '`' && first != '_' && 
		first != '^' && first != '{' && first != '|' && first != '}') {
		return (false);
	}

	// Rest can be letters, digits, or special chars
	for (std::size_t i = 1; i < nick.length(); ++i) {
		char c = nick[i];
		if (!std::isalnum(c) && c != '[' && c != ']' && c != '\\' && 
			c != '`' && c != '_' && c != '^' && c != '{' && c != '|' && 
			c != '}' && c != '-') {
			return (false);
		}
	}
	return (true);
}

std::string		CommandRouter::formatChannelUserList(Channel& channel) const {
	std::list<Client*> clients = channel.getClientList();
	std::string userList;
	
	for (std::list<Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (it != clients.begin()) {
			userList += " ";
		}
		
		// Add @ prefix for operators
		if (channel.isOperator(**it)) {
			userList += "@";
		}
		
		userList += (*it)->get_nick();
	}
	
	return (userList);
}