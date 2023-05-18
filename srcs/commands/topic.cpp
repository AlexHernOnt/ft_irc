/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

//TODO
void Server::Command_topic( int client_sd, std::string data )
{
    std::cout << "ENTRA AL TOPIC EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;

    //TODO: cuando se pone como TOPIC : se esta pidiendo el tópico el cual devuelve 
    //                                      331 RPL_NOTOPIC "<channel> :No topic is set"
    //                                      o
    //                                      332 RPL_TOPIC "<channel> :<topic>"
    //TODO: cuando se pone como TOPIC :<tópico> se está intentando cambiar el tópico (concepto)
    //                                      El canal puede tener una restricción para que solo operadores puedan cambiar
}
