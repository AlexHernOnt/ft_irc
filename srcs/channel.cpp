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
    channel_key = "";
    private_channel = false;
    secret_channel = false;
    invite_only = false;
    topic_opr_only = false;
    out_msg_reject = false;
    moderated_channel = false;
    channel_max_users = -1; //mientras sea -1 no hay límite
}

Channel::~Channel( void )
{
}

Channel::Channel( const Channel& other )
{
    *this = other;
}

int Channel::JoinClient( int client_sd, bool oprtor )
{
    std::vector<int>::iterator i_client = std::find(client_sd_list.begin(), client_sd_list.end(), client_sd);
    if (i_client != client_sd_list.end())
        return 1; //ya existe
    client_sd_list.push_back(client_sd);
    if (oprtor == true || oprtor_sd_list.size() <= 0)
        oprtor_sd_list.push_back(client_sd);
    return 0; //ok
}

bool Channel::PartClient( int client_sd )
{
    //lista de clientes
    std::vector<int>::iterator i_client = std::find(client_sd_list.begin(), client_sd_list.end(), client_sd);
    if (i_client != client_sd_list.end())
        client_sd_list.erase(i_client);
    else
        return false;

    //lista de operadores
    i_client = std::find(oprtor_sd_list.begin(), oprtor_sd_list.end(), client_sd);
    if (i_client != oprtor_sd_list.end())
        oprtor_sd_list.erase(i_client);

    return true;
}

void Channel::SetChannelConcept( std::string new_concept )
{
    channel_concept = new_concept;
}

std::string Channel::GetChannelConcept( void )
{
    return channel_concept;
}

std::vector<int> Channel::GetClients( void )
{
    return client_sd_list;
}

std::vector<int> Channel::GetNoOperatorClients( void )
{
    std::vector<int> no_operators;
    bool in_operators;
    for (unsigned long i = 0; i < client_sd_list.size(); i++)
    {
        in_operators = false;
        for (unsigned long j = 0; j < oprtor_sd_list.size(); j++)
        {
            if (client_sd_list[i] == oprtor_sd_list[j])
            {
                in_operators = true;
                break;
            }
        }
        if (in_operators == false)
            no_operators.push_back(client_sd_list[i]);
    }
    return no_operators;
}

std::vector<int> Channel::GetOperators( void )
{
    return oprtor_sd_list;
}

bool Channel::GetIfClientOperator( int client_sd )
{
    for (unsigned long i = 0; i < oprtor_sd_list.size(); i++)
    {
        if (oprtor_sd_list[i] == client_sd)
            return true;
    }
    return false;
}

bool Channel::GetIfClientInChannel( int client_sd )
{
    for (unsigned long i = 0; i < client_sd_list.size(); i++)
    {
        if (client_sd_list[i] == client_sd)
            return true;
    }
    return false;
}

bool Channel::CheckKey( std::string trying_key )
{
    if (channel_key == "")
        return true;
    if (trying_key == channel_key)
        return true;
    return false;
}
