/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

//TODO
void Server::Command_kick( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL KICK EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    std::ostringstream oss;

    std::vector<std::string> split_msg = Split(data, ":");
	std::vector<std::string> split_inputs = Split(split_msg[0], " ");
    if (client_list[client_sd].registered == false) //no registrado
	{
		oss << ":You have not registered";
		ServerMsgToClient(client_sd, "451", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if ((split_inputs.size() + split_msg.size() - 1) < 3) //parámetros insuficientes
	{
		oss << "KICK :Not enough parameters";
		ServerMsgToClient(client_sd, "461", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    std::map<std::string, Channel>::iterator channel = channels_list.find(split_inputs[1]);
    if (channel == channels_list.end())
	{
		oss << split_inputs[1] << " :No such channel";
		ServerMsgToClient(client_sd, "403", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (channel->second.GetIfClientInChannel(client_sd) == false)
	{
		oss << split_inputs[1] << " :You're not on that channel";
		ServerMsgToClient(client_sd, "442", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (channel->second.GetIfClientOperator(client_sd) == false)
	{
		oss << split_inputs[1] << " :You're not channel operator";
		ServerMsgToClient(client_sd, "482", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    int target_client_sd = GetClientSdByNick(split_inputs[2]);
    if (target_client_sd != -1)
    {
        std::string msg;
        if (split_msg.size() > 1)
            msg = split_msg[1];
        else if (split_inputs.size() > 3)
            msg = split_inputs[3];
        else
            msg = client_list[client_sd].nick;

        oss << "KICK " << split_inputs[1] << " " << split_inputs[2] << " :" << msg;
        SendClientMsgToChannel(client_sd, oss.str(), split_inputs[1]);
        oss.str("");
        oss.clear();

        channel->second.PartClient(target_client_sd);

        //borrar de la lista de canales guardado en el cliente
        for (unsigned long j = 0; j < client_list[target_client_sd].channels_joined.size(); j++)
        {
            if (client_list[target_client_sd].channels_joined[j] == split_inputs[1])
            {
                client_list[target_client_sd].channels_joined.erase(client_list[target_client_sd].channels_joined.begin() + j);
                break;
            }
        }
    }
}
