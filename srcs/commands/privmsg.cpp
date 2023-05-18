/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_privmsg( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL PRIVMSG EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    std::ostringstream oss;

    std::vector<std::string> split_msg = Split(data, ":");
	std::vector<std::string> split_inputs = Split(split_msg[0], " ");
    std::string msg;

    if (client_list[client_sd].registered == false) //no registrado
	{
		oss << ":You have not registered";
		ServerMsgToClient(client_sd, "451", oss.str());
		oss.str("");
		oss.clear();
		return;
    }
    if ((split_inputs.size() + split_msg.size() - 1) < 2) //solo ha escrito PRIVMSG
	{
		oss << ":No recipient given (PRIVMSG)";
		ServerMsgToClient(client_sd, "411", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if ((split_inputs.size() + split_msg.size() - 1) < 3) //no ha escrito el mensaje
	{
		oss << ":No text to send";
		ServerMsgToClient(client_sd, "412", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (split_msg.size() >= 2)
        msg = split_msg[1];
    else
        msg = split_inputs[2];

    std::vector<std::string> split_targets = Split(split_inputs[1], ",");

    for (unsigned long i = 0; i < split_targets.size(); i++)
    {
        if (CheckChannelName(split_targets[i]) == true)
        {
            //TODO: el mensaje no se puede enviar al canal
            oss << "PRIVMSG " << split_targets[i] << " :" << msg;
            SendClientMsgToChannel(client_sd, oss.str(), split_targets[i], false);
            oss.str("");
            oss.clear();
        }
        else
        {
            int target_client_sd = GetClientSdByNick(split_targets[i]);

            if (target_client_sd == -1)
            {
                oss << split_targets[i] << " :No such nick/channel";
                ServerMsgToClient(client_sd, "401", oss.str());
                oss.str("");
                oss.clear();
                continue;
            }

            oss << "PRIVMSG " << split_targets[i] << " :" << msg;
            SendClientMsg(client_sd, oss.str(), target_client_sd);
            oss.str("");
            oss.clear();
        }
    }
}
