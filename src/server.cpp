/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmontoro <gmontoro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:54:16 by gmontoro          #+#    #+#             */
/*   Updated: 2025/10/13 14:54:17 by gmontoro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"
#include "../client.hpp"
#include "../command_router.hpp"
#include "../channel.hpp"
#include <cerrno>

Server::Server(int p, std::string pa){
	port = p;
	pass = pa;
	sock = init_server_socket();
	dir = init_dir(port);
	bind(sock, (sockaddr *)&dir, sizeof(dir));
	listen(sock, 1024);
	poll_server();
	_parser = Parser();
	_commandRouter = new CommandRouter(*this);
}

Server::~Server() {
	for (std::vector<Channel*>::iterator it = channel_list.begin(); it != channel_list.end(); ++it) {
		delete *it;
	}
	for (std::vector<Client*>::iterator it = client_list.begin(); it != client_list.end(); ++it) {
		delete *it;
	}
	delete _commandRouter;
}

int	Server::init_server_socket(){
	sock = socket(AF_INET, SOCK_STREAM, 0);//AF_INET for ipv4; SOCK_STREAM for TCP (SOCK_DGRAM for UDP)
	if (sock == -1)
		exit(1);
	int	i = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));//SOL_SOCKET indicates that we're adjusting the socket options
	fcntl(sock, F_SETFL, O_NONBLOCK);//sets the socket to NON_BLOCK, so the program doest get stunned when theres no data in a socket
	return (sock);
}

sockaddr_in	Server::init_dir(int port){
	sockaddr_in address;

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;//for IPV4
	address.sin_port = htons(port);//changes to network byte order(16 bits)
	address.sin_addr.s_addr = INADDR_ANY;//to accept any addres
	return (address);
}

void  Server::poll_server(){
	struct pollfd pfd;
	pfd.fd = this->sock;
	pfd.events = POLLIN;
	pfd.revents = 0;
	this->pollfd.push_back(pfd);
}

int	Server::monitor_poll(){
	int	res;
	res = poll(&pollfd[0], pollfd.size(), -1);
	if (res == 0)
		return (std::cout << "timeout\n", -1);
	return (res);//returns number of polls that have revents
}

int  Server::who_is_event(){
	std::size_t i = 0;
	while (i < pollfd.size()){
		struct pollfd &pfd = pollfd[i];
		if (pfd.revents == 0){
			++i;
			continue;
		}

		if (pfd.fd == sock && (pfd.revents & POLLIN)){
			std::cout << "Created new client\n";
			add_new_client();
			++i;
			continue;
		}

		int fd = pfd.fd;

		if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)){
			std::cout << "[who_is_event] removing fd due to error/hup/nval: " << fd << "\n";
			removeClientByFd(fd);
			continue;
		}

		Client *client = getClientByFd(fd);
		if (!client){
			std::cout << "[who_is_event] no Client* for fd, removing fd=" << fd << "\n";
			removeClientByFd(fd);
			continue;
		}

		std::string res = handle_message(fd, client);
		if (res.empty()){
			std::cout << "[who_is_event] handle_message signalled close, removing fd=" << fd << "\n";
			removeClientByFd(fd);
			continue;
		}

		++i;
	}
	return (0);
}

void  Server::add_new_client(){
	sockaddr_in	address;
	socklen_t	len  = sizeof(address);
	int	sockfd;

	sockfd = accept(sock, (sockaddr *)&address, &len);
	if (sockfd < 0)
		exit(21);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	struct pollfd pfd;
	pfd.fd = sockfd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	this->pollfd.push_back(pfd);
	// Changed to use heap allocation instead of stack as we need Client instances to persist
	Client* new_client = new Client(sockfd, address);
	client_list.push_back(new_client);
}

std::string  Server::handle_message(int fd, Client *client){
	int a;
	char	aux[1024];
	std::string	buff;
	std::string	line;

	a = recv(fd, aux, sizeof(aux) - 1, 0);
	if (a == 0) {
		std::cout << "Client ended connection\n";
		return ("");
	}
	if (a < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK){
			std::cout << "[handle_message] EAGAIN/EWOULDBLOCK on fd=" << fd << "\n";
			return ("EAGAIN");
		}
		std::cout << "[handle_message] recv error on fd=" << fd << " errno=" << errno << "\n";
		return ("");
	}

	std::string received_data(aux, a);
	client->appendToBuffer(received_data);

	while (!(line = client->extractCompleteMessage()).empty()) {
		Message msg = _parser.parseMessage(line);
		CommandRouter::CommandResult result = _commandRouter->processCommand(fd, msg);

		if (result == CommandRouter::CMD_DISCONNECT) {
			// signal close to caller; centralized removal in who_is_event
			return ("");
		}
	}

	return (buff.empty() ? std::string("OK") : buff);
}

std::vector<struct pollfd>	Server::get_pollfd(){
	return pollfd;
}

// Add extra utility methods/ getters to aid integration with other components
// Client management
Client*				Server::getClientByFd(int fd) {
	for (std::vector<Client*>::iterator it = client_list.begin(); it != client_list.end(); ++it) {
		if ((*it)->get_sock_fd() == fd) {
			return (*it);
		}
	}

	return (NULL);
}

Client*				Server::getClientByNick(const std::string& nick) {
	for (std::vector<Client*>::iterator it = client_list.begin(); it != client_list.end(); ++it) {
		if ((*it)->get_nick() == nick) {
			return (*it);
		}
	}

	return (NULL);
}

void				Server::removeClientByFd(int fd) {
	for (std::vector<Client*>::iterator it = client_list.begin(); it != client_list.end(); ++it) {
		if ((*it)->get_sock_fd() == fd) {
			delete *it;
			client_list.erase(it);
			break ;
		}
	}

	// Remove from pollfd vector
	for (std::vector<struct pollfd>::iterator it = pollfd.begin(); it != pollfd.end(); ++it) {
		if (it->fd == fd) {
			close(fd);
			pollfd.erase(it);
			break ;
		}
	}
}

// Channel management
Channel*			Server::getChannelByName(const std::string& name) {
	for (std::vector<Channel*>::iterator it = channel_list.begin(); it != channel_list.end(); ++it) {
		if ((*it)->getName() == name) {
			return (*it);
		}
	}

	return (NULL);
}

Channel*			Server::createChannel(const std::string& name) {
	Channel* channel = new Channel(name);

	channel_list.push_back(channel);

	return (channel);
}

void				Server::removeChannel(const std::string& name) {
	for (std::vector<Channel*>::iterator it = channel_list.begin(); it != channel_list.end(); ++it) {
		if ((*it)->getName() == name && (*it)->getClientCount() == 0) {
			delete *it;
			channel_list.erase(it);
			break ;
		}
	}
}

// Server information access
const std::string&	Server::getPassword(void) const {
	return (pass);
}