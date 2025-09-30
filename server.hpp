#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <cstdio>
# include <poll.h>
# include <list>
# include <vector>
# include <netinet/in.h>//pa sockaddr_in
# include <stdint.h>
# include "client.hpp"
# include "channel.hpp"
# include "command_router.hpp"
# include "parser.hpp"

 class Client;

class Server{
	private:
		int	port;
		std::string pass;
		int	sock;
		sockaddr_in	dir;//direccion de socket ipv4
		std::vector<struct pollfd> pollfd;//tendremos un pollfd para el server
		//y luego uno por cliente q se conecte
		std::vector<Client *> client_list;

		// Add extra for channel/ commands integration
		std::vector<Channel*>	channel_list;
		Parser					_parser;
		CommandRouter*			_commandRouter;

	public:
		Server(int, std::string);
		int	init_server_socket();
		int	monitor_poll();
		sockaddr_in	init_dir(int);
		std::vector<struct pollfd>	get_pollfd();
		void	poll_server();
		int	who_is_event();
		void	add_new_client();
		std::string	handle_message(int, Client *);
		void	handlePassCommand(Client *, const std::string&);
		Client get_client(int);
		~Server();

		// Add extra utility methods/ getters to aid integration with other components
		// Client management
		Client*				getClientByFd(int fd);
		Client*				getClientByNick(const std::string& nick);
		void				removeClientByFd(int fd);

		// Channel management
		Channel*			getChannelByName(const std::string& name);
		Channel*			createChannel(const std::string& name);
		void				removeChannel(const std::string& name);

		// Server information access
		const std::string&	getPassword(void) const;
};

#endif