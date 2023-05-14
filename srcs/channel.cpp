/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"

Channel::Channel( void )
{
}

Channel::~Channel( void )
{
}

Channel::Channel( const Channel& other )
{
    *this = other;
}

int Channel::JoinClient( int client_sd )
{
    std::vector<int>::iterator i_client = std::find(client_sd_list.begin(), client_sd_list.end(), client_sd);
    if (i_client != client_sd_list.end())
        return 1; //ya existe
    client_sd_list.push_back(client_sd);
    return 0; //ok
}

void Channel::PartClient( int client_sd )
{
    std::vector<int>::iterator i_client = std::find(client_sd_list.begin(), client_sd_list.end(), client_sd);
    if (i_client != client_sd_list.end())
        client_sd_list.erase(i_client);
}

void Channel::SetChannelConcept( std::string new_concept )
{
    channel_concept = new_concept;
}

std::string Channel::GetChannelConcep( void )
{
    return channel_concept;
}

std::vector<int> Channel::GetClients( void )
{
    return client_sd_list;
}
