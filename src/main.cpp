/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmontoro <gmontoro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 13:28:33 by gmontoro          #+#    #+#             */
/*   Updated: 2025/09/16 12:22:52 by gmontoro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"


//esto es el server, por ahora, si


int main(int argc, char **argv)
{
	if (argc != 3)
		return (1);
	std::string pass(argv[2]);
	//int c = 0;
	Server server(std::atoi(argv[1]), pass);//inicializamos socket, address, las bindeamos
									//y 
	while (1){
		//poll es un socket grande policia. solo puede haber una llamada 
		if (server.monitor_poll() > 0)//habemus data -> comprobamos q fd lo ha recibido
		{
			server.who_is_event();
		}
	}
	return (0);
}
