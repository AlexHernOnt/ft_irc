/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/21 23:21:52 by rgirondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_topic( int client_sd, std::string data )
{
    std::ostringstream oss;

	std::vector<std::string> split_msg = SplitMsg(data);
    std::vector<std::string> split_inputs = Split(data, " ");
    std::string msg;

    if (client_list[client_sd].registered == false) // no registrado
	{
		oss << ":You have not registered";
		ServerMsgToClient(client_sd, "451", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (split_inputs.size() < 2) // no parameters
    {
        oss << "TOPIC :Not enough parameters";
        ServerMsgToClient(client_sd, "461", oss.str());
        oss.str("");
        oss.clear();
        return ;
    }

    msg = split_inputs[2];
    if (split_msg.size() > 1 && split_inputs.size() > 2)
        msg = split_msg[1];
    
    if (split_inputs.size() > 1) // command
    {
        std::map<std::string, Channel>::iterator ch_it = channels_list.find(split_inputs[1]);
        if (ch_it != channels_list.end() && (!(ch_it->second).GetS_Flag() || (ch_it->second).GetIfClientInChannel(client_sd)))
        {
            if (!(ch_it->second).GetIfClientInChannel(client_sd)) // client not in the channel
            {
                oss << ch_it->first << " :You're not on that channel";
                ServerMsgToClient(client_sd, "442", oss.str());
                oss.str("");
                oss.clear();
                return ;
            }
            if (split_inputs.size() > 2 && ((ch_it->second).GetT_Flag() || (ch_it->second).GetIfClientOperator(client_sd)))
                (ch_it->second).SetChannelConcept(msg);
            else if (split_inputs.size() > 2) // no operator priv
            {
                oss << " :Permission Denied- You're not an IRC operator";
                ServerMsgToClient(client_sd, "481", oss.str());
                oss.str("");
                oss.clear();
                return ;
            }
            if(((ch_it->second).GetChannelConcept()).size() != 0) // topic
            {
                oss << ch_it->first << " :";
                if (!(ch_it->second).GetP_Flag() || (ch_it->second).GetIfClientInChannel(client_sd))
                    oss << ((ch_it->second)).GetChannelConcept();
                if (split_inputs.size() < 3)
                    ServerMsgToClient(client_sd, "332", oss.str());
                else
                    SendClientMsgToChannel(client_sd, oss.str(), split_inputs[1]);
                oss.str("");
                oss.clear();
            }
            else // empty topic
            {
                oss << ch_it->first << " :No topic is set";
                ServerMsgToClient(client_sd, "331", oss.str());
                oss.str("");
                oss.clear();
                return ;
            }
        }
        if(ch_it == channels_list.end())// channel doesnt exist
        {
            oss << split_inputs[1] << " :You're not on that channel";
            ServerMsgToClient(client_sd, "442", oss.str());
            oss.str("");
            oss.clear();
            return ;
        }
    }

    //TODO: cuando se pone como TOPIC : se esta pidiendo el tópico el cual devuelve 
    //                                      331 RPL_NOTOPIC "<channel> :No topic is set"
    //                                      o
    //                                      332 RPL_TOPIC "<channel> :<topic>"
    //TODO: cuando se pone como TOPIC :<tópico> se está intentando cambiar el tópico (concepto)
    //                                      El canal puede tener una restricción para que solo operadores puedan cambiar
}
