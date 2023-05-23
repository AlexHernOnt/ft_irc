/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_oper( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL OPER EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    std::ostringstream oss;

	std::vector<std::string> split_inputs = Split(data, " ");

    if (client_list[client_sd].registered == false) //no registrado
	{
		oss << ":You have not registered";
		ServerMsgToClient(client_sd, "451", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (split_inputs.size() < 3) //parámetros insuficientes
	{
		oss << "OPER :Not enough parameters";
		ServerMsgToClient(client_sd, "461", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (OPR_USER == "") //no existen sesiones guardadas de operadores en el servidor
	{
		oss << ":No O-lines for your host";
		ServerMsgToClient(client_sd, "491", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (split_inputs[2] != OPR_PASSWORD)
    {
        oss << ":Password incorrect";
		ServerMsgToClient(client_sd, "464", oss.str());
		oss.str("");
		oss.clear();
		return;
    }

    client_list[client_sd].oprtor = true;

    oss << ":You are now an IRC operator";
    ServerMsgToClient(client_sd, "381", oss.str());
    oss.str("");
    oss.clear();
}