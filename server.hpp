#ifndef SERVER_HPP
#define SERVER_HPP

 #include <iostream>
 #include <string.h>
 #include <fcntl.h>
 #include <sys/socket.h>
 #include <unistd.h>
 #include <stdlib.h>
 #include <cstdio>
 #include <poll.h>
 #include <list>
 #include <vector>
 #include <netinet/in.h>//pa sockaddr_in
 #include <stdint.h>

class Server{
	private:
		int	port;
		std::string pass;
		int	sock;
		sockaddr_in	dir;//direccion del server
		std::vector<struct pollfd> pollfd;//tendremos un pollfd para el server
		//y luego uno por cliente q se conecte
		//std::list<Client &> client_list;
	public:
		Server(int, std::string);
		int	init_server_socket();
		int	monitor_poll();
		sockaddr_in	init_dir(int);
		std::vector<struct pollfd>	get_pollfd();
		void	poll_server();
		int	who_is_event();
		void	new_client();
		std::string	handle_message(int);
		~Server();
};
#endif