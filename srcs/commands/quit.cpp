/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_quit( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL QUIT EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    std::ostringstream oss;

	std::vector<std::string> split_inputs = Split(data, " ");
    std::string quit_msg;

    if (client_list[client_sd].registered == false) //no registrado
	{
		oss << ":You have not registered";
		ServerMsgToClient(client_sd, "451", oss.str());
		oss.str("");
		oss.clear();
		return;
	}

    oss << "ERROR :Closing Link: " << client_list[client_sd].nick << "[~" << client_list[client_sd].username << "@" << client_list[client_sd].ip_address << "] ";
	if (split_inputs.size() >= 2) //tiene mensaje
		quit_msg = split_inputs[1];
    else
        quit_msg = client_list[client_sd].nick;
    oss << "(" <<  quit_msg << ")" << std::endl;
    OtherMsgToClient(client_sd, oss.str());
    oss.str("");
	oss.clear();

    oss << " QUIT :Quit: :" << quit_msg;
    for (unsigned long i = 0; i < client_list[client_sd].channels_joined.size(); i++)
    {
        std::string channel_str = client_list[client_sd].channels_joined[i];
        SendClientMsgToChannel(client_sd, oss.str(), channel_str);
        channels_list[channel_str].PartClient( client_sd );
    }
    close_conn = true;
}
