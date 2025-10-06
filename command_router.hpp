/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_router.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmontoro <gmontoro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 12:55:53 by sstoev            #+#    #+#             */
/*   Updated: 2025/09/29 16:26:16 by gmontoro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_ROUTER_HPP
# define COMMAND_ROUTER_HPP

# include <string>
# include <vector>
# include <map>
# include "message.hpp"

class	Server;
class	Client;
class	Channel;

class	CommandRouter {
	public:
		// Command execution results
		enum CommandResult {
			CMD_OK,				// cmd successfully processed
			CMD_ERROR,			// cmd failed but client still connected
			CMD_DISCONNECT,		// client should be disconnected
			CMD_INVALID			// cmd malformed
		};

	private:
		// Define funciton pointer for command handlers
		typedef CommandResult (CommandRouter::*CommandHandler)(Client&, const Message&);

		// Define dispatch table that maps command names to handler methods
		std::map<std::string, CommandHandler>	_commandHandlers;
		Server&	_server;

		// Private copy constructor/ assignment op to prevent unregulated calls, i.e. make them uncallable (no implementation in .cpp file)
		CommandRouter(const CommandRouter& other);
		CommandRouter&	operator=(const CommandRouter& other);

		// Initialize dispatch table;
		void	initCommandHandlers(void);

		// Helper methods for validation/ authentication
		bool	requiresRegistration(const std::string& command) const;
		bool	requiresAuthentication(const std::string& command) const;
	
	public:
		CommandRouter(Server& server);
		~CommandRouter(void);

		// Main entry point
		CommandResult	processCommand(int client_fd, const Message& msg);

		// Command handlers - return appropriate CommandResult
		CommandResult	handlePASS(Client& client, const Message& msg);
		CommandResult	handleNICK(Client& client, const Message& msg);
		CommandResult	handleUSER(Client& client, const Message& msg);
		CommandResult	handleJOIN(Client& client, const Message& msg);
		CommandResult	handlePART(Client& client, const Message& msg);
		CommandResult	handleTOPIC(Client& client, const Message& msg);
		CommandResult	handleMODE(Client& client, const Message& msg);
		CommandResult	handleKICK(Client& client, const Message& msg);
		CommandResult	handleINVITE(Client& client, const Message& msg);
		CommandResult	handlePRIVMSG(Client& client, const Message& msg);
		CommandResult	handleNOTICE(Client& client, const Message& msg);
		CommandResult	handlePING(Client& client, const Message& msg);
		CommandResult	handlePONG(Client& client, const Message& msg);
		CommandResult	handleQUIT(Client& client, const Message& msg);
		CommandResult	handleCAP(Client& client, const Message& msg);
		
		// Public utils
		void			sendResponse(Client& client, const std::string& response);
		void			sendError(Client& client, const std::string& code, const std::string& message);
		bool			validateChannelName(const std::string& name) const;
		bool			validateNickname(const std::string& nick) const;
		std::string		formatChannelUserList(Channel& channel) const;
};

#endif