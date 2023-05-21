/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/21 20:53:48 by rgirondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

//TODO
void Server::Command_topic( int client_sd, std::string data )
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

    if (split_inputs.size() < 2) //no parameters
    {
        oss << "TOPIC :Not enough parameters";
        ServerMsgToClient(client_sd, "461", oss.str());
        oss.str("");
        oss.clear();
        return ;
    }

    if (split_inputs.size() == 2)
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

    //TODO: cuando se pone como TOPIC : se esta pidiendo el tópico el cual devuelve 
    //                                      331 RPL_NOTOPIC "<channel> :No topic is set"
    //                                      o
    //                                      332 RPL_TOPIC "<channel> :<topic>"
    //TODO: cuando se pone como TOPIC :<tópico> se está intentando cambiar el tópico (concepto)
    //                                      El canal puede tener una restricción para que solo operadores puedan cambiar
}
