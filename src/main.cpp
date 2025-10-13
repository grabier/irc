/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmontoro <gmontoro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 13:28:33 by gmontoro          #+#    #+#             */
/*   Updated: 2025/10/13 14:01:20 by gmontoro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		return (1);
	std::string pass(argv[2]);
	if (std::atoi(argv[1]) < 1024 || std::atoi(argv[1]) > 65535)
		return (std::cerr << "Error: invalid port\n", 1);
	Server server(std::atoi(argv[1]), pass);
	while (1){
		if (server.monitor_poll() > 0)
			server.who_is_event();
	}
	return (0);
}
