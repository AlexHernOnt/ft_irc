/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_nick( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL NICK EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    std::ostringstream oss;

	std::vector<std::string> split_inputs = Split(data, " ");

    if (split_inputs.size() < 2) //no se ha encontrado el nick en los parámetros
	{
		oss << ":No nickname given";
		ServerMsgToClient(client_sd, "431", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (split_inputs[1] == "anonymous" /* TODO: comprobar si el nick no vale por reglas de escritura */)
	{
		oss << split_inputs[1] << " :Erroneous Nickname";
		ServerMsgToClient(client_sd, "432", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    /*
    ERR_NICKCOLLISION
        "<nick> :Nickname collision KILL"

        - Returned by a server to a client when it detects a
            nickname collision (registered of a NICK that
            already exists by another server).

        como especifica ANOTHER SERVER y no tenemos conexión server a server he supuesto que no hay por que ponerlo
    */
    if (GetClientSdByNick(split_inputs[1]) != -1) //si se encuentra el nick duplicado en la base de datos
    {
        oss << split_inputs[1] << " :Nickname is already in use";
        ServerMsgToClient(client_sd, "433", oss.str());
        oss.str("");
        oss.clear();
        return;
    }

    //ok continuar
    client_list[client_sd].nick_set = true;
    client_list[client_sd].nick = split_inputs[1];

    if (client_list[client_sd].registered == false)
    {
        if (client_list[client_sd].user_set == false)
            return;
        if (client_list[client_sd].password_ok == false && password != "")
        {
            oss << ":Password incorrect";
            ServerMsgToClient(client_sd, "464", oss.str());
            oss.str("");
            oss.clear();

            oss << "ERROR :Closing Link: " << client_list[client_sd].ip_address << " (Bad Password)" << std::endl;
            OtherMsgToClient(client_sd, oss.str());
            oss.str("");
            oss.clear();

            close_conn = true;
            return;
        }
        client_list[client_sd].registered = true;
        WelcomeClient(client_sd);
    }
}
