#pragma once

#include "server.hpp"

class Client{
	private:
		int	sock_fd;
		//int	port;
		sockaddr_in	address;
		std::string nick;
		std::string user;
		std::string real_name;
		int	registered;
	public:
		Client(int fd, sockaddr_in addr);
		int get_sock_fd();
		int get_register_status();
		std::string get_nick();
		std::string get_user();
		~Client();
};
