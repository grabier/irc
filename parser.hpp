/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sstoev <sstoev@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 23:35:18 by sstoev            #+#    #+#             */
/*   Updated: 2025/09/18 23:39:03 by sstoev           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include "message.hpp"

class	Parser {
	public:
		Parser(void);
		~Parser(void);

		// Parse a raw message string into a Message object
		Message	parseMessage(const std::string& rawMessage) const;

		// Validate message components
		bool	isValidCommand(const std::string& command) const;
};

#endif
