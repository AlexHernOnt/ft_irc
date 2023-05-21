/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/21 19:15:41 by rgirondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

//TODO
void Server::Command_mode( int client_sd, std::string data )
{
    std::cout << "ENTRA AL MODE EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    
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
    
    if (CheckChannelName(split_inputs[1]) == true)
    {
        //mode channel
        if (split_inputs.size() == 2)
        {
            //TODO: get info
        }
        else
        {

        }
    }
    else
    {
        //mode client
        if (split_inputs[1] != client_list[client_sd].nick) //si no coincide el nick con el suyo
        {
            oss << ":Cant change mode for other users";
            ServerMsgToClient(client_sd, "502", oss.str());
            oss.str("");
            oss.clear();
            return;
        }

        if (split_inputs.size() == 2)
        {
            oss << GetUserFlags(client_sd);
            ServerMsgToClient(client_sd, "221", oss.str());
            oss.str("");
            oss.clear();
            return;
        }
        else
        {
            ProcessClientFlags(client_sd, split_inputs[2]);
        }
    }

    /*
        ERR_CHANOPRIVSNEEDED            ERR_NOSUCHNICK
        ERR_NOTONCHANNEL                ERR_KEYSET
        RPL_BANLIST                     RPL_ENDOFBANLIST
        ERR_UNKNOWNMODE                 ERR_NOSUCHCHANNEL

        ERR_UMODEUNKNOWNFLAG
    */
}
