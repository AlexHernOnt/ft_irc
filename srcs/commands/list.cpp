/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 19:26:17 by rgirondo          #+#    #+#             */
/*   Updated: 2023/05/21 18:56:52 by rgirondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_list( int client_sd, std::string data )
{
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

    oss << "Channel :Users  Name";
    ServerMsgToClient(client_sd, "321", oss.str());
    oss.str("");
	oss.clear();

    if (split_inputs.size() > 1)
    {
        std::vector<std::string> split_channels = Split(split_inputs[1], ",");
        for (unsigned int i = 0; i != split_channels.size(); i++)
        {
            std::map<std::string, Channel>::iterator ch_it = channels_list.find(split_channels[i]);
            if (ch_it != channels_list.end() && (!(ch_it->second).GetS_Flag() || (ch_it->second).GetIfClientInChannel(client_sd)))
            {
                oss << ch_it->first << " " << ((ch_it->second).GetClients()).size() << " :";
                if (!(ch_it->second).GetP_Flag() || (ch_it->second).GetIfClientInChannel(client_sd))
                    oss << ((ch_it->second)).GetChannelConcept();
    		    ServerMsgToClient(client_sd, "322", oss.str());
        	    oss.str("");
		        oss.clear();
            }
        }
    }
    else
    {
        for (std::map<std::string, Channel>::iterator it = channels_list.begin(); it != channels_list.end(); it++)
        {
            oss << it->first << " " << ((it->second).GetClients()).size() << " :" << (it->second).GetChannelConcept();
    		ServerMsgToClient(client_sd, "322", oss.str());
        	oss.str("");
		    oss.clear();
        }
    }

    oss << ":End of /LIST";
    ServerMsgToClient(client_sd, "323", oss.str());
    oss.str("");
	oss.clear();
}