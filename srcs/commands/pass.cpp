/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/25 18:47:34 by rgirondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_pass( int client_sd, std::string data )
{
    //std::cout << "ENTRA AL PASS EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;
    std::ostringstream oss;

	std::vector<std::string> split_inputs = Split(data, " ");

	if (split_inputs.size() < 2) //parámetros insuficientes
	{
		oss << "PASS :Not enough parameters";
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
    if (password != split_inputs[1]) //contraseña incorrecta. Creo que no es necesario el mensaje aquí
	{
		/*oss << ":Password incorrect";
		ServerMsgToClient(client_sd, "464", oss.str());
		oss.str("");
		oss.clear();*/
		std::cout << "PRIMERO " << split_inputs[0] << std::endl;
		std::cout << "COMPARA " << split_inputs[1] << "/" << password << std::endl;
		return;
	}
    //ok continuar
    client_list[client_sd].password_ok = true;
}
