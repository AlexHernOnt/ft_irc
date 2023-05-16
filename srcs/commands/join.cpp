/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_join( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL JOIN EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
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

    if (split_inputs.size() < 2) //parámetros insuficientes
	{
		oss << "JOIN :Not enough parameters";
		ServerMsgToClient(client_sd, "461", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    std::vector<std::string> channels = Split(split_inputs[1], ",");
    std::map<std::string,Channel>::iterator it;

    for (unsigned long i = 0; i < channels.size(); i++)
    {
        //TODO: comprobar que esté bien escrito y eso
        client_list[client_sd].channels_joined.push_back(channels[i]);
        it = channels_list.find(channels[i]);
        if (it == channels_list.end()) //crear nuevo si no existe
        {
            //TODO: comprobar si el channel empieza por # o & y que no tenga espacios, ^G o comas
            channels_list.insert(std::pair<std::string, Channel>(channels[i], Channel()));
            it = channels_list.find(channels[i]);
        }
        it->second.JoinClient( client_sd, false );

        //aviso JOIN a todos los del canal
        oss << "JOIN :" << channels[i];
        SendClientMsgToChannel(client_sd, oss.str(), channels[i]);
        oss.str("");
		oss.clear();

        oss << "= " << channels[i] << " :";
        std::vector<int> channel_users = it->second.GetOperators();
        //operadores
        for (unsigned long j = 0; j < channel_users.size(); j++)
        {
            oss << "@" << client_list[channel_users[j]].nick << " ";
        }
        //no operadores
        channel_users = it->second.GetNoOperatorClients();
        for (unsigned long j = 0; j < channel_users.size(); j++)
        {
            oss << "+" << client_list[channel_users[j]].nick << " ";
        }
		ServerMsgToClient(client_sd, "353", oss.str());
		oss.str("");
		oss.clear();

        oss << channels[i] << " :End of /NAMES list.";
		ServerMsgToClient(client_sd, "366", oss.str());
		oss.str("");
		oss.clear();
    }
}
