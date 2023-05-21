/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_notice( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL NOTICE EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    std::ostringstream oss;

    std::vector<std::string> split_msg = SplitMsg(data);
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
    if ((split_inputs.size() + split_msg.size() - 1) < 2) //solo ha escrito NOTICE
	{
		oss << ":No recipient given (NOTICE)";
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

    int target_client_sd = GetClientSdByNick(split_inputs[1]);

    if (target_client_sd != -1)
    {
        oss << "NOTICE " << split_inputs[1] << " :" << msg;
        SendClientMsg(client_sd, oss.str(), target_client_sd);
        oss.str("");
        oss.clear();
    }
}
