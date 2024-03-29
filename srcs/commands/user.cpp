/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_user( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL USER EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
	std::ostringstream oss;

	std::vector<std::string> split_realname = SplitMsg(data);	//0: resto de datos 1: realname
	std::vector<std::string> split_inputs = Split(split_realname[0], " ");	//si no hay realname con : se toma la posicion 4

	if ((split_inputs.size() + split_realname.size() - 1) < 5) //parámetros insuficientes
	{
		oss << "USER :Not enough parameters";
		ServerMsgToClient(client_sd, "461", oss.str());
		oss.str("");
		oss.clear();
		return;
	}
	if (client_list[client_sd].registered == true) //ya registrado
	{
		
		oss << ":You may not reregister";
		ServerMsgToClient(client_sd, "462", oss.str());
		oss.str("");
		oss.clear();
		return;
	}
    //ok continuar
	client_list[client_sd].user_set = true;
	client_list[client_sd].username = split_inputs[1];
	if (split_realname.size() > 1)
		client_list[client_sd].realname = split_realname[1];
	else
		client_list[client_sd].realname = split_inputs[4];

	if (client_list[client_sd].registered == false)
    {
        if (client_list[client_sd].nick_set == false)
            return;
        if (client_list[client_sd].password_ok == false && password != "")
        {
            oss << ":Password incorrect";
            ServerMsgToClient(client_sd, "464", oss.str());
            oss.str("");
            oss.clear();

            oss << "ERROR :Closing Link: " << client_list[client_sd].ip_address << " (Bad Password)" << std::endl;
            OtherMsgToClient(client_sd, oss.str());
            oss.str("");
            oss.clear();

            close_conn = true;
            return;
        }
        client_list[client_sd].registered = true;
        WelcomeClient(client_sd);
    }
}

void Server::WelcomeClient( int client_sd )
{
	std::ostringstream oss;

	oss << ":There are " << client_list.size() << " users and 0 services on 1 servers";
	ServerMsgToClient(client_sd, "251", oss.str());
	oss.str("");
	oss.clear();

	oss << GetOperatorCount() << " :operators online";
	ServerMsgToClient(client_sd, "252", oss.str());
	oss.str("");
	oss.clear();

	oss << GetUnregisteredCount() << " :unknown connections";
	ServerMsgToClient(client_sd, "253", oss.str());
	oss.str("");
	oss.clear();

	oss << channels_list.size() << " :channels formed";
	ServerMsgToClient(client_sd, "254", oss.str());
	oss.str("");
	oss.clear();

	oss << ":I have " << client_list.size() << " users and 0 services on 1 servers";
	ServerMsgToClient(client_sd, "255", oss.str());
	oss.str("");
	oss.clear();

	oss << ":- " << HOST_NAME << " Message of the Day -";
	ServerMsgToClient(client_sd, "375", oss.str());
	oss.str("");
	oss.clear();

	//mensaje del día
	oss << ":- There is no message of the day.";
	ServerMsgToClient(client_sd, "372", oss.str());
	oss.str("");
	oss.clear();

	oss << ":- Just continue.";
	ServerMsgToClient(client_sd, "372", oss.str());
	oss.str("");
	oss.clear();
	//fin mensaje

	oss << ":End of MOTD command.";
	ServerMsgToClient(client_sd, "376", oss.str());
	oss.str("");
	oss.clear();

	oss << ":" << client_list[client_sd].nick << " MODE " << client_list[client_sd].nick << " :" << GetUserFlags(client_sd) << std::endl;
	OtherMsgToClient(client_sd, oss.str());
	oss.str("");
	oss.clear();
	/*
	:albatrosstrosst MODE albatrosstrosst :+i
	*/
}
