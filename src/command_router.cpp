/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_router.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sstoev <sstoev@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:15:09 by sstoev            #+#    #+#             */
/*   Updated: 2025/09/19 22:57:27 by sstoev           ###   ########.fr       */
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
	_commandHandlers["PASS"] = &CommandRouter::handlePASS;
	_commandHandlers["NICK"] = &CommandRouter::handleNICK;
	_commandHandlers["USER"] = &CommandRouter::handleUSER;
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

	// Check authentication requirements
	if (!client->isAuthenticated() && requiresAuthentication(command)) {
		sendError(*client, "464", ":Password incorrect");
		
		return (CMD_ERROR);
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
 
CommandRouter::CommandResult	CommandRouter::handlePASS(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] PASS handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleNICK(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] NICK handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleUSER(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] USER handler called (stub)" << std::endl;
	
	return (CMD_OK);
}


// ----------------------------- CHANNEL OPERATION HANDLERS -----------------------------

CommandRouter::CommandResult	CommandRouter::handleJOIN(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] JOIN handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handlePART(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] PART handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleTOPIC(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] TOPIC handler called (stub)" << std::endl;
	
	return (CMD_OK);
}


// ----------------------------- CHANNEL MODERATION HANDLERS -----------------------------

CommandRouter::CommandResult	CommandRouter::handleMODE(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] MODE handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleKICK(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] KICK handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleINVITE(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] INVITE handler called (stub)" << std::endl;
	
	return (CMD_OK);
}


// ----------------------------- CHANNEL MODERATION HANDLERS -----------------------------

CommandRouter::CommandResult	CommandRouter::handlePRIVMSG(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] PRIVMSG handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleNOTICE(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] NOTICE handler called (stub)" << std::endl;
	
	return (CMD_OK);
}


// ----------------------------- CHANNEL CONNECTION MANAGEMENT HANDLERS -----------------------------

CommandRouter::CommandResult	CommandRouter::handlePING(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] PING handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handlePONG(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] PONG handler called (stub)" << std::endl;
	
	return (CMD_OK);
}

CommandRouter::CommandResult	CommandRouter::handleQUIT(Client& client, const Message& msg) {
	// Stub code just so that it compiles
	// TODO: Provide full implementation
	(void)client; (void)msg;
	
	std::cout << "[CommandRouter] QUIT handler called (stub)" << std::endl;
	
	return (CMD_OK);
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
	// TODO: Basic validation only - full implementation when integrated
	if (name.empty() || name[0] != '#') {
		return (false);
	}

	// Arbitrary limit char limit set just to be on the safe side, change if needed; protocol doesn't explicitly specify it
	if (name.length() > 100) {
		return (false);
	}

	return (true);
}

bool			CommandRouter::validateNickname(const std::string& nick) const {
	// TODO: Basic validation only - full implementation when integrated
	// nick size is again set arbitrarily as a safety net
	if (nick.empty() || nick.length() > 30) {
		return (false);
	}

	return (!nick.empty());
}

std::string		CommandRouter::formatChannelUserList(Channel& channel) const {
	// Stub implementation - can skip if not deemed necessary
	(void)channel;

	return ("user_list_placeholder");
}