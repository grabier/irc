#include "../server.hpp"
 #include "../client.hpp"

Server::Server(int p, std::string pa){
	port = p;
	pass = pa;
	sock = init_server_socket();//el socket es el punto de comunicacion
	dir = init_dir(port);
	bind(sock, (sockaddr *)&dir, sizeof(dir));//enlaza socket con address(direccion + puerto)
	//escuchamos->listen marca al servidor listo para aceptar conexiones
	listen(sock, 1024);//habra q meter las q queramos. numero de conexiones en cola
	poll_server();//inicializamos el pollfd del server

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
	sock = socket(AF_INET, SOCK_STREAM, 0);//configuramos la comunicacion q queremos: 
	//el primer param puede ser AF_INET para ipv4, AF_INET6 para ipv6, o AF_LOCAL
	//para conexiones entre proceesos de la misma makina
	//la segunda flag es SOCK_STREAM para que vaya por TCP o SOCK_DGRAM si quisieramos UDP
	if (sock == -1)
		exit(1);
	int	i = 1;
	//setsockopt ajusta las opciones a nivel del socket(SOL_SOCKET), y lo seteamos a 
	//SO_REUSEADDR, pra que podamos reutilizar direccion y puerto aunque el socket anterior
	//siga bloqueado(en estado TIME_WAIT). la i una flag de activacion para el tercer parametro
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
	//fcntl manipula el fd sock para activar el modo no bloqueante, 
	//para q si hacemos un read en un socket y no hay datos, el programa no se quede
	//bloqueado esperando
	fcntl(sock, F_SETFL, O_NONBLOCK);//pa que no haga syscals donde no debe
	return (sock);
}

sockaddr_in	Server::init_dir(int port){
	sockaddr_in address;//un sockadd_in es una struct q representa una direccion de 
	//socket ipv4. contiene: 
	//sin_family->familia de direcciones (AF_INET para ipv4 en nuestro caso)
	//sin_port->puerto, en 16 bits (network byte order)
	//sin_addr->direccion ipv4 (32 bits)
	memset(&address, 0, sizeof(address));//limpiamos la struct para inicializarla
	address.sin_family = AF_INET;//pa ipv4
	address.sin_port = htons(port);//pq sockadd_in se come un 16 bytes
	address.sin_addr.s_addr = INADDR_ANY;//pa que se coma cualquier ip
	//si quisieramos una dir especifica se la especificamos ahi
	return (address);
}

void  Server::poll_server(){
	struct pollfd pfd;//events es el input, revents output
	pfd.fd = this->sock;
	pfd.events = POLLIN;//le digo q lea
	pfd.revents = 0;
	this->pollfd.push_back(pfd);
}

int	Server::monitor_poll(){
	int	res;
	res = poll(&pollfd[0], pollfd.size(), -1);
	/* if (res == -1)
		return (-1); */
	if (res == 0)
		return (std::cout << "timeout\n", -1);
	return (res);//numero de pollfd q tienen revents validos
}

int  Server::who_is_event(){
	unsigned int	i = 0;
	//std::cout << "DEBUGAMOS\n";
	std::vector<struct pollfd>::iterator it;
	it = pollfd.begin();
	while(i < pollfd.size()){//recorremos todos los eventos
		if (pollfd[i].revents > 0){
			if (pollfd[i].fd == sock){
				//checkeamos el fd asociado al server
				//si el evento es en el server->alguien intenta conectarr
				//alguien ha puesto bien el puerto y la ip
				std::cout << "created a cliento\n";
				add_new_client();
			}
			else
			{
				std::cout << "handle a cliento\n";
				if (handle_message(pollfd[i].fd, client_list[i - 1]) == "")
					pollfd.erase(it + i);
			}
		}
		//buscar por revents = 0 else
		i++;
	}
	return (0);
}

void  Server::add_new_client(){
	sockaddr_in	address;
	socklen_t	len  = sizeof(address);
	int	sockfd;
	//devuelve el fd del socket del cliente
	sockfd = accept(sock, (sockaddr *)&address, &len);//la misma idea q socket pero pal cliente
	//configuramos el socket del cliente.
	if (sockfd < 0)
		exit(21);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	//aqui creariamos el cliente y añadirlo a la lista
	struct pollfd pfd;//creamos el pollfd del cliente y lo añadimos a la lista
	pfd.fd = sockfd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	this->pollfd.push_back(pfd);

	// Changed to use heap allocation instead of stack as we need Client instances to persist
	Client* new_client = new Client(sockfd, address);
	client_list.push_back(new_client);//añadimos a la lista
	//std::cout << "created a cliento\n";
}

std::string  Server::handle_message(int fd, Client *client){
	int a;
	char	aux[1024];
	std::string	buff;
	std::string	line;
	//size_t	pos = 0;
	while (1)
	{
		a = recv(fd, aux, sizeof(aux) - 1, 0);//recive los mensajes en aux
		if (a == 0) {
			std::cout << "cliente finaliza\n";

			// Instead of exit(1), handle client disconnect gracefully to prevent memory leaks
			removeClientByFd(fd);
			return ("");
		}

		std::string	line(aux);
		client->appendToBuffer(line);
		line = client->extractCompleteMessage();
		if (line != "")
		{
			Message msg = _parser.parseMessage(line);
			CommandRouter::CommandResult result = _commandRouter->processCommand(fd, msg);
			if (result == CommandRouter::CMD_ERROR){
				//std::cout << "sale x aki\n";
				//line.erase(0, line.size());
				return "";
			}
			// Handle cmd results
			if (result == CommandRouter::CMD_DISCONNECT) {
				removeClientByFd(fd);
				return (buff);
			}
		}

		//es basicamente el read para sockets
	/* 	std::string	line(aux);
		client->appendToBuffer(line); */
		/* while ((pos = buff.find("\n")) != std::string::npos && buff != ""){

			std::cout << "line: " << line << std::endl;
			
			// Parse and process command
			if (!line.empty()) {
				Message msg = _parser.parseMessage(line);
				CommandRouter::CommandResult result = _commandRouter->processCommand(fd, msg);
				if (result == CommandRouter::CMD_ERROR){
					//std::cout << "sale x aki\n";
					//line.erase(0, line.size());
					return "";
				}
				// Handle cmd results
				if (result == CommandRouter::CMD_DISCONNECT) {
					removeClientByFd(fd);
					return (buff);
				}
			}
		} */
		/* while ((pos = buff.find("\r\n"))) {
			line = buff.substr(0, pos);
			buff.erase(0, pos + 2); // +2 para quitar \r\n
			//std::cout << line << std::endl;
		} */
		//std::cout << "patatatataaat: " << line << std::endl;
		//break;
		//std::cout << "DEBUGAMOS\n";
		/* if (aux[a - 1] == '\n' && aux[a - 2] == '\t'){//creo q hay q checkear por /t/n
			std::cout << aux;
			break ;
		} */
	}
	return (buff);
}

/* Client Server::get_client(int fd){
	for(int i = 0; i < client_list.size(); i++){
		if (fd == client_list[i]->get_sock_fd()){
			if (client_list[i]->get_register_status() == 0)//si no esta registrado(nick + user), pedimos documentasiao
				ask_for_authentication();
		}
	}
} */

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
