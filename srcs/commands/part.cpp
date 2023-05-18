/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_part( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL PART EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
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
		oss << "PART :Not enough parameters";
		ServerMsgToClient(client_sd, "461", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    std::vector<std::string> channels = Split(split_inputs[1], ",");
    std::map<std::string,Channel>::iterator it;

    for (unsigned long i = 0; i < channels.size(); i++)
    {
        it = channels_list.find(channels[i]);
        if (it != channels_list.end())
        {
            if (it->second.PartClient(client_sd) == false) //al intentar quitarlo no ha aparecido
            {
                oss << channels[i] << " :You're not on that channel";
                ServerMsgToClient(client_sd, "442", oss.str());
                oss.str("");
                oss.clear();
                continue;
            }
            else
            {
                oss << "PART " << channels[i];
                SendClientMsg(client_sd, oss.str(), client_sd); //porque lo he quitado antes. Si es que soy un chapuzas
                SendClientMsgToChannel(client_sd, oss.str(), channels[i]);
                oss.str("");
                oss.clear();

                //borrar de la lista de canales guardado en el cliente
                for (unsigned long j = 0; j < client_list[client_sd].channels_joined.size(); j++)
                {
                    if (client_list[client_sd].channels_joined[j] == channels[i])
                    {
                        client_list[client_sd].channels_joined.erase(client_list[client_sd].channels_joined.begin() + j);
                        break;
                    }
                }
            }

            //borrar el canal si no queda nadie
            if (channels_list[channels[i]].GetClients().size() <= 0)
            {
                channels_list.erase(channels_list.find(channels[i]));
            }
        }
        else
        {
            oss << channels[i] << " :No such channel";
            ServerMsgToClient(client_sd, "403", oss.str());
            oss.str("");
            oss.clear();
            continue;
        }
    }
}
