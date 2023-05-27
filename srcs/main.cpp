/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int main(int argc , char *argv[])  
{  
	Server	server;
	int		point = 1;
	int		aux_port;

	if (argc < 3)
	{
		std::cout << "Insuficent params" << std::endl;
		std::cout << "Params: [host:port_network:password_network UNUSED] <port> <password>" << std::endl;
		return 1;
	}

	if (argc >= 4)
		point++;

	try
	{
		aux_port = std::stoi(argv[point]);
	}
	catch(std::exception &err)
	{
		std::cout << "Port is not a number." << std::endl;
		return 1;
	}
		
	server.SetServerData( argv[point + 1], aux_port );

    server.ServerSocketSetup();

	server.MainLoop();

	server.Cleanfds();

	return 0;
}
