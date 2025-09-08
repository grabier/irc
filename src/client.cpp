#include "../client.hpp"

Client::Client(int _port, std::string _nick, int _socket){
	port = _port;
	nick = _nick;
	sock = _socket;
}

Client::~Client(){}

//192.168.x.x reserved

//10.x.x.x reserved

//hay otra,, rango random 

//   /28->>>> 255.255.255.0 o algo asi. hay calculadoras