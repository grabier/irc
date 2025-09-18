/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sstoev <sstoev@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 22:28:44 by sstoev            #+#    #+#             */
/*   Updated: 2025/09/18 22:28:47 by sstoev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <cstddef>
# include <string>
# include <vector>

class Message {
	private:
		std::string	_prefix;
		std::string	_command;
		std::vector<std::string>	_params;

	public:
		Message(void);
		Message(const std::string& prefix, const std::string& command,
				const std::vector<std::string>& params);
		~Message(void);

		// Getters
		std::string	getPrefix(void) const;
		std::string	getCommand(void) const;
		std::vector<std::string> getParams(void) const;
		std::string getParamAt(std::size_t index) const;
		std::size_t	getParamCount(void) const;

		// For constructing response strings
		std::string	toString(void) const;
};

#endif
