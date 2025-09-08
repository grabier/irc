#include "../server.hpp"

int	Server::monitor_poll(){
	int	res;
	res = poll(&pollfd[0], pollfd.size(), -1);
	if (res == -1)
		return (-1);
	/* if (res == 0)
		return (std::cout << "timeout\n", -1); */
	return (res);//numero de pollfd q tienen revents validos
}

std::string  Server::handle_message(int fd){
	int a;
	char	aux[1024];
	std::string	buff;
	while (1)
	{
		a = recv(fd, aux, 1024, 0);//recive los mensajes en aux
		//es basicamente el read para sockets
		if (a == 0){
			std::cout << "cliente finaliza\n";
			exit (1);
		}
		//std::cout << "DEBUGAMOS\n";
		if (aux[a - 1] == '\n'){
			std::cout << aux;
			break ;
		}
		buff.append(aux);
	}
	return (buff);
}


void  Server::new_client(){
	sockaddr_in	address;
	socklen_t	len  = sizeof(address);
	int	sockfd;
	//devuelve el fd del socket del cliente
	sockfd = accept(sock, (sockaddr *)&address, &len);//la misma idea q socket pero pal cliente
	if (sockfd < 0)
		exit(21);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	//aqui creariamos el cliente y añadirlo a la lista
	struct pollfd pfd;//creamos el pollfd del cliente y lo añadimos a la lista
	pfd.fd = sockfd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	this->pollfd.push_back(pfd);
	//std::cout << "created a cliento\n";
}

int  Server::who_is_event(){
	unsigned int	i = 0;
	//std::cout << "DEBUGAMOS\n";
	while(i < pollfd.size()){//recorremos todos los eventos
		if (pollfd[i].revents > 0){
			if (pollfd[i].fd == sock){//si el evento es en el server->alguien intenta conectarr
				//alguien ha puesto bien el puerto y la ip
				std::cout << "created a cliento\n";
				new_client();
			}
			else{//el evento es en un cliente ->comandos y demas
				std::cout << "handle a cliento\n";
				handle_message(pollfd[i].fd);
			}
		}
		i++;
	}
	return (0);
}

void  Server::poll_server(){
	struct pollfd pfd;//events es el input, revents output
	pfd.fd = this->sock;
	pfd.events = POLLIN;//le digo q lea
	pfd.revents = 0;
	this->pollfd.push_back(pfd);
}

sockaddr_in	Server::init_dir(int port){
	sockaddr_in address;
	memset(&address, 0, sizeof(address));//limpiamos address
	address.sin_family = AF_INET;//pa ipv4
	address.sin_port = htons(port);//pq sockadd_in se come un 16 bytes
	address.sin_addr.s_addr = INADDR_ANY;//pa que se coma cualquier ip
	//si quisieramos una dir especifica se la especificamos ahi
	return (address);
}

int	Server::init_server_socket(){
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		exit(1);
	int	i = 1;
	//config socket: sol para ip, 
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
	fcntl(sock, F_SETFL, O_NONBLOCK);//pa que no haga syscals donde no debe
	return (sock);
}

Server::Server(int p, std::string pa){
	port = p;
	pass = pa;
	sock = init_server_socket();
	dir = init_dir(port);
	bind(sock, (sockaddr *)&dir, sizeof(dir));//enlaza socket con address
	//escuchamos
	listen(sock, 1024);//habra q meter las q queramos
	poll_server();
}

std::vector<struct pollfd>	Server::get_pollfd(){
	return pollfd;
}

Server::~Server(){}

