/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sstoev <sstoev@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 22:44:19 by sstoev            #+#    #+#             */
/*   Updated: 2025/09/18 23:00:17 by sstoev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../message.hpp"

Message::Message(void) : _prefix(""), _command("") { }

Message::Message(const std::string& prefix, const std::string& command,
				const std::vector<std::string>& params)
	: _prefix(prefix), _command(command), _params(params) { }

Message::~Message(void) { }

// Getters
std::string	Message::getPrefix(void) const {
	return (_prefix);
}

std::string	Message::getCommand(void) const {
	return (_command);
}

std::vector<std::string> Message::getParams(void) const {
	return (_params);
}

std::string Message::getParamAt(std::size_t index) const {
	if (index < _params.size()) {
		return (_params[index]);
	}

	return ("");
}

std::size_t	Message::getParamCount(void) const {
	return (_params.size());
}

// For constructing response strings
std::string	Message::toString(void) const {
	std::string	result;

	if (!_prefix.empty()) {
		result += ":" + _prefix + " ";
	}

	result += _command;

	for (std::size_t i = 0; i < _params.size(); ++i) {
		// Check if the last parameter contains a space
		// According to RFC 1459 if the last param contains spaces, it must be predixed with : to prevent it from being split
		if (i == _params.size() - 1 && _params[i].find(' ') != std::string::npos)
			result += " :" + _params[i];
		else
			result += " " + _params[i];
	}
	result += "\r\n";
	
	return (result);
}