/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmontoro <gmontoro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 23:35:34 by sstoev            #+#    #+#             */
/*   Updated: 2025/09/29 17:02:06 by gmontoro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"
#include <cctype>
#include <sstream>
#include <iostream>

Parser::Parser(void) { }

Parser::~Parser(void) { }

// Parse a raw message string into a Message object
Message	Parser::parseMessage(const std::string& rawMessage) const {
	std::string	prefix;
	std::string	command;
	std::vector<std::string>	params;
	//std::cout << "line: " << rawMessage << std::endl;
	// Remove trailing CR+LF if present
	std::string message = rawMessage;
	if (message.size() >= 2 &&
		message[message.size() - 2] == '\r' &&
		message[message.size() - 1] == '\n') {
			message = message.substr(0, message.size() - 2);
	}
	//std::cout << "line: " << message << std::endl;
	// Handle empty message
	if (message.empty()) {
		return (Message("", "", std::vector<std::string>()));
	}

	// Parse prefix if present
	std::size_t pos = 0;
	if (message[0] == ':') {
		pos = message.find(' ');
		if (pos == std::string::npos) {
			// Invalid message format
			// TODO: revisit later if better handling needed once we centralise error utility, currently we handle it silently
			return (Message("", "", std::vector<std::string>()));
		}
		prefix = message.substr(1, pos - 1);
		// Skip the space
		pos++;
	}

	// Extract command
	std::size_t nextPos = message.find(' ', pos);
	if (nextPos == std::string::npos) {
		// No params case
		command = message.substr(pos);
	}
	else {
		// Params case
		command = message.substr(pos, nextPos - pos);
		pos = nextPos + 1;

		// Extract parameters
		while (pos < message.size()) {
			// Check for trailing parameter (starts with :)
			if (message[pos] == ':') {
				params.push_back(message.substr(pos + 1));
				break ;
			}
			
			nextPos = message.find(' ', pos);
			if (nextPos == std::string::npos) {
				params.push_back(message.substr(pos));
				break ;
			}

			else {
				params.push_back(message.substr(pos, nextPos - pos));
				pos = nextPos + 1;
			}
		}
	}
	//std::cout << "params: " << params[0] << std::endl;
	// Convert command to uppercase (IRC commands are case-insensitive)
	std::string uppercaseCommand = command;
	for (std::size_t i = 0; i < uppercaseCommand.size(); i++) {
		uppercaseCommand[i] = std::toupper(static_cast<unsigned char>(uppercaseCommand[i]));
	}
	//std::cout << "command: " << uppercaseCommand << std::endl;
	return (Message(prefix, uppercaseCommand, params));
}

// Validate message components
bool	Parser::isValidCommand(const std::string& command) const {
	// List of valid IRC commands based on project scope
	// Defined as static on purpose- initialized only once, shared across all instances
	// C-style string preferred here to std::string due to minimal memory overhead
	static const char* validCommands[] = {
		"PASS", "NICK", "USER", "JOIN", "PART", "PRIVMSG",
		"NOTICE", "KICK", "INVITE", "TOPIC", "MODE", "QUIT",
		"PING", "PONG", "CAP", NULL};
	
	for (int i = 0; validCommands[i] != NULL; i++) {
		if (command == validCommands[i]) {
			return (true);
		}
	}

	return (false);
}
