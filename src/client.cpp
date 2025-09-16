#include "../client.hpp"

Client::Client(int fd, sockaddr_in add){
	address = add;
	sock_fd = fd;
	registered = 0;
}

std::string Client::get_nick(){
	return (nick);
}

std::string Client::get_user(){
	return (user);
}

int Client::get_sock_fd(void){
	return (sock_fd);
}

int Client::get_register_status(void){
	return (registered);
}

Client::~Client(){}

//192.168.x.x reserved

//10.x.x.x reserved

//hay otra,, rango random 

//   /28->>>> 255.255.255.0 o algo asi. hay calculadoras