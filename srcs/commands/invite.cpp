/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/23 20:29:41 by rgirondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_invite( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL PRIVMSG EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    std::ostringstream oss;
    std::vector<std::string> split_inputs = Split(data, " ");

    if (client_list[client_sd].registered == false) //not registered ERR_NOTREGISTERED
	{
		oss << ":You have not registered";
		ServerMsgToClient(client_sd, "451", oss.str());
		oss.str("");
		oss.clear();
		return;
    }

    if (split_inputs.size() < 3) // no parameters ERR_NEEDMOREPARAMS
    {
        oss << "INVITE :Not enough parameters";
        ServerMsgToClient(client_sd, "461", oss.str());
        oss.str("");
        oss.clear();
        return ;
    }

    std::map<std::string, Channel>::iterator ch_it = channels_list.find(split_inputs[2]);
    
    if (!(ch_it->second).GetIfClientInChannel(client_sd)) // client not in the channel ERR_NOTONCHANNEL
    {
        oss << ch_it->first << " :You're not on that channel";
        ServerMsgToClient(client_sd, "442", oss.str());
        oss.str("");
        oss.clear();
        return ;
    }

    if ((ch_it->second).GetP_Flag() == true && (ch_it->second).GetIfClientOperator(client_sd) == false) // no privileges to invite in this channel ERR_CHANOPRIVSNEEDED
    {
        oss << ch_it->first << " :You're not channel operator";
        ServerMsgToClient(client_sd, "482", oss.str());
        oss.str("");
        oss.clear();
        return ;
    }

    int target_client_sd = GetClientSdByNick(split_inputs[1]);

    if (target_client_sd == -1 || ch_it == channels_list.end())// user desnt exist ERR_NOSUCHNICK
    {
        oss << split_inputs[1] << " :No such nick/channel";
        ServerMsgToClient(client_sd, "401", oss.str());
        oss.str("");
        oss.clear();
        return;
    }

    if ((ch_it->second).GetIfClientInChannel(target_client_sd)) // user already in the channel ERR_USERONCHANNEL
    {
        oss << split_inputs[1] << " " << ch_it->first << " :is already on channel";
        ServerMsgToClient(client_sd, "443", oss.str());
        oss.str("");
        oss.clear();
        return ;
    }

    if (ch_it != channels_list.end()) // if channel exists send invitation RPL_INVITING
    {
        (ch_it->second).SendInvitation(target_client_sd);
        oss << split_inputs[1] << " " << ch_it->first;
        ServerMsgToClient(client_sd, "341", oss.str());
        SendClientMsg(client_sd, oss.str(), target_client_sd);
        oss.str("");
        oss.clear();
    }
}
