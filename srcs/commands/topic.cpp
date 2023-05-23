/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/23 20:02:11 by rgirondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_topic( int client_sd, std::string data )
{
    std::ostringstream oss;

	std::vector<std::string> split_msg = SplitMsg(data);
    std::vector<std::string> split_inputs = Split(data, " ");
    std::string msg;

    if (client_list[client_sd].registered == false) // no registered ERR_NOTREGISTERED
	{
		oss << ":You have not registered";
		ServerMsgToClient(client_sd, "451", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    if (split_inputs.size() < 2) // no parameters ERR_NEEDMOREPARAMS
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
            if (!(ch_it->second).GetIfClientInChannel(client_sd)) // client not in the channel ERR_NOTONCHANNEL
            {
                oss << ch_it->first << " :You're not on that channel";
                ServerMsgToClient(client_sd, "442", oss.str());
                oss.str("");
                oss.clear();
                return ;
            }
            if (split_inputs.size() > 2 && ((ch_it->second).GetT_Flag() || (ch_it->second).GetIfClientOperator(client_sd)))
                (ch_it->second).SetChannelConcept(msg);
            else if (split_inputs.size() > 2) // no operator priv ERR_CHANOPRIVSNEEDED
            {
                oss << ch_it->first << " :You're not channel operator";
                ServerMsgToClient(client_sd, "482", oss.str());
                oss.str("");
                oss.clear();
                return ;
            }
            if(((ch_it->second).GetChannelConcept()).size() != 0) // topic RPL_TOPIC
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
            else // empty topic RPL_NOTOPIC
            {
                oss << ch_it->first << " :No topic is set";
                ServerMsgToClient(client_sd, "331", oss.str());
                oss.str("");
                oss.clear();
                return ;
            }
        }
        if(ch_it == channels_list.end())// channel doesnt exist ERR_NOTONCHANNEL
        {
            oss << split_inputs[1] << " :You're not on that channel";
            ServerMsgToClient(client_sd, "442", oss.str());
            oss.str("");
            oss.clear();
            return ;
        }
    }
}
