/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_user( int client_sd, std::string data )
{
    std::cout << "ENTRA AL USER EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;

    //comprobar todo y luego sacar welcome si ok
    WelcomeClient(client_sd);
}

void Server::WelcomeClient( int client_sd )
{
	std::ostringstream oss;

	oss << ":There are " << client_list.size() << " users and 0 services on 1 servers";
	ServerMsgToClient(client_sd, "251", oss.str());
	oss.str("");
	oss.clear();

	//TODO: número de operadores
	oss << client_list.size() << " :operators online";
	ServerMsgToClient(client_sd, "252", oss.str());
	oss.str("");
	oss.clear();

	//TODO: número de conexiones anónimas/sin registrar
	oss << client_list.size() << " :unknown connections";
	ServerMsgToClient(client_sd, "253", oss.str());
	oss.str("");
	oss.clear();

	//TODO: número de canales
	oss << client_list.size() << " :channels formed";
	ServerMsgToClient(client_sd, "254", oss.str());
	oss.str("");
	oss.clear();

	oss << ":I have " << client_list.size() << " users and 0 services on 1 servers";
	ServerMsgToClient(client_sd, "255", oss.str());
	oss.str("");
	oss.clear();

	oss << ":- " << HOST_NAME << " Message of the Day -";
	ServerMsgToClient(client_sd, "375", oss.str());
	oss.str("");
	oss.clear();

	//TODO: mensaje del día (varias líneas seguramente)
	oss << ":(Message of the Day)";
	ServerMsgToClient(client_sd, "372", oss.str());
	oss.str("");
	oss.clear();

	oss << ":End of MOTD command.";
	ServerMsgToClient(client_sd, "376", oss.str());
	oss.str("");
	oss.clear();

	/*
	:albatrosstrosst MODE albatrosstrosst :+i
	*/
}
