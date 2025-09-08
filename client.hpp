#pragma once

#include "server.hpp"

class Client{
	private:
		int	port;
		std::string nick;
		int	sock;
	public:
		Client(int, std::string, int);
		~Client();
};
