#include "../server.hpp"

Server::Server(int p, std::string pa){
	port = p;
	pass = pa;
	sock = init_server_socket();//el socket es el punto de comunicacion
	dir = init_dir(port);
	bind(sock, (sockaddr *)&dir, sizeof(dir));//enlaza socket con address(direccion + puerto)
	//escuchamos->listen marca al servidor listo para aceptar conexiones
	listen(sock, 1024);//habra q meter las q queramos. numero de conexiones en cola
	poll_server();
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
		if (aux[a - 1] == '\n'){//creo q hay q checkear por /t/n
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




std::vector<struct pollfd>	Server::get_pollfd(){
	return pollfd;
}

Server::~Server(){}

