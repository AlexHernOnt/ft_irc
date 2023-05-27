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

    std::vector<std::string> channel_keys;
    if (split_inputs.size() >= 3)
        channel_keys = Split(split_inputs[2], ",");

    for (unsigned long i = 0; i < channels.size(); i++)
    {
        if (CheckChannelName(channels[i]) == false)
        {
            oss << channels[i] << " :No such channel";
            ServerMsgToClient(client_sd, "403", oss.str());
            oss.str("");
            oss.clear();
            continue;
        }

        for (unsigned long j = 0; j < client_list[client_sd].channels_joined.size(); j++)
        {
            if (client_list[client_sd].channels_joined[j] == channels[i])
                return;
        }

        if (client_list[client_sd].channels_joined.size() >= 10)
        {
            oss << channels[i] << " :You have joined too many \\ channels";
            ServerMsgToClient(client_sd, "405", oss.str());
            oss.str("");
            oss.clear();
            return;
        }

        it = channels_list.find(channels[i]);
        if (it == channels_list.end()) //crear nuevo si no existe
        {
            channels_list.insert(std::pair<std::string, Channel>(channels[i], Channel()));
            it = channels_list.find(channels[i]);
        }
        else
        {
            std::string try_key = "";
            if (i < channel_keys.size())
                try_key = channel_keys[i];
            if (it->second.CheckKey(try_key) == false)
            {
                //ERR_BADCHANNELKEY (password)
                oss << channels[i] << " :Cannot join channel (+k)";
                ServerMsgToClient(client_sd, "475", oss.str());
                oss.str("");
                oss.clear();
                continue;
            }
        }

        if (it->second.GetIfNickBanned( client_list[client_sd].nick ) == true)
        {
            oss << channels[i] << " :Cannot join channel (+b)";
            ServerMsgToClient(client_sd, "474", oss.str());
            oss.str("");
            oss.clear();
            return;
        }
        
        if (it->second.GetIfFull() == true)
        {
            oss << channels[i] << " :Cannot join channel (+l)";
            ServerMsgToClient(client_sd, "471", oss.str());
            oss.str("");
            oss.clear();
            return;
        }

        if (it->second.GetI_Flag() == true && it->second.GetIfClientInvited(client_sd) == false)
        {
            oss << channels[i] << " :Cannot join channel (+i)";
            ServerMsgToClient(client_sd, "473", oss.str());
            oss.str("");
            oss.clear();
            return;
        }
        client_list[client_sd].channels_joined.push_back(channels[i]);
        it->second.JoinClient( client_sd, false );

        //aviso JOIN a todos los del canal
        oss << "JOIN :" << channels[i];
        SendClientMsgToChannel(client_sd, oss.str(), channels[i]);
        oss.str("");
		oss.clear();

        if (it->second.GetChannelConcept() != "")
        {
            oss << channels[i] << " :" << it->second.GetChannelConcept();
            ServerMsgToClient(client_sd, "332", oss.str());
            oss.str("");
            oss.clear();
        }

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
