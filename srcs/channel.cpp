/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/23 20:05:01 by rgirondo         ###   ########.fr       */
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

void Channel::MakeOperator( int client_sd )
{
    if (GetIfClientOperator(client_sd) == true)
        return;
    oprtor_sd_list.push_back(client_sd);
}

void Channel::UnmakeOperator( int client_sd )
{
    std::vector<int>::iterator i_client = std::find(oprtor_sd_list.begin(), oprtor_sd_list.end(), client_sd);
    if (i_client != oprtor_sd_list.end())
        oprtor_sd_list.erase(i_client);
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

bool Channel::GetIfClientCanSpeak( int client_sd )
{
    std::vector<int>::iterator i_client = std::find(oprtor_sd_list.begin(), oprtor_sd_list.end(), client_sd);
    if (i_client != oprtor_sd_list.end())
        return true;
    i_client = std::find(moderated_channel_allowed.begin(), moderated_channel_allowed.end(), client_sd);
    if (i_client != moderated_channel_allowed.end())
        return true;
    return false;
}

bool Channel::GetIfClientInvited ( int client_sd )
{
    std::vector<int>::iterator i_client = std::find(invited_clients_sd.begin(), invited_clients_sd.end(), client_sd);
    if (i_client == invited_clients_sd.end())
        return false;
    return true;
}

void Channel::RemoveInvited ( int client_sd )
{
    std::vector<int>::iterator i_client = std::find(invited_clients_sd.begin(), invited_clients_sd.end(), client_sd);
    if (i_client != invited_clients_sd.end())
        invited_clients_sd.erase(i_client);
}

bool Channel::CheckKey( std::string trying_key )
{
    if (channel_key == "")
        return true;
    if (trying_key == channel_key)
        return true;
    return false;
}

void Channel::SetKey( std::string new_key )
{
    channel_key = new_key;
}

bool Channel::GetIfNickBanned( std::string nick )
{
    for (unsigned long i = 0; i < ban_mask_list.size(); i++)
    {
        if (nick == ban_mask_list[i])
            return true;
    }
    return false;
}

std::vector<std::string> Channel::GetBanMasks( void )
{
    return ban_mask_list;
}

void Channel::SetBanMask( std::string mask )
{
    std::vector<std::string>::iterator i_mask = std::find(ban_mask_list.begin(), ban_mask_list.end(), mask);
    if (i_mask == ban_mask_list.end())
        ban_mask_list.push_back(mask);
}

void Channel::RemoveBanMask( std::string mask )
{
    std::vector<std::string>::iterator i_mask = std::find(ban_mask_list.begin(), ban_mask_list.end(), mask);
    if (i_mask != ban_mask_list.end())
        ban_mask_list.erase(i_mask);
}

void Channel::AllowUserMsg( int client_sd )
{
    std::vector<int>::iterator i_client = std::find(moderated_channel_allowed.begin(), moderated_channel_allowed.end(), client_sd);
    if (i_client == moderated_channel_allowed.end())
        moderated_channel_allowed.push_back(client_sd);
}

void Channel::DeclineUserMsg( int client_sd )
{
    std::vector<int>::iterator i_client = std::find(moderated_channel_allowed.begin(), moderated_channel_allowed.end(), client_sd);
    if (i_client != moderated_channel_allowed.end())
        moderated_channel_allowed.erase(i_client);
}

std::string Channel::GetChannelFlags( void )
{
    std::ostringstream oss;
    oss << "+";
    /*
        p - private channel flag;
        s - secret channel flag;
        i - invite-only channel flag;
        t - topic settable by channel operator only flag;
        n - no messages to channel from clients on the outside;
        m - moderated channel;
        l - set the user limit to channel;
        b - set a ban mask to keep users out;
        v - give/take the ability to speak on a moderated channel;
        k - set a channel key (password).
    */
            
    if (private_channel == true)
		oss << "p";
    if (secret_channel == true)
		oss << "s";
    if (invite_only == true)
		oss << "i";
    if (topic_opr_only == true)
		oss << "t";
    if (out_msg_reject == true)
		oss << "n";
    if (moderated_channel == true)
		oss << "m";
    if (channel_max_users != -1)
		oss << "l";
	if (channel_key != "")
		oss << "k";

    if (channel_max_users != -1)
		oss << " " << channel_max_users;
	if (channel_key != "")
		oss << " " << channel_key;

    return oss.str();
}

bool Channel::GetIfFull( void )
{
    if (channel_max_users == -1)
        return false;
    unsigned long m_users = channel_max_users;
    if (m_users < client_sd_list.size())
        return false;
    return true;
}

bool    Channel::GetP_Flag( void )
{
    return this->private_channel;
}

bool    Channel::GetS_Flag( void )
{
    return this->secret_channel;
}

bool    Channel::GetI_Flag( void )
{
    return this->invite_only;
}

bool    Channel::GetT_Flag( void )
{
    return this->topic_opr_only;
}

bool    Channel::GetN_Flag( void )
{
    return this->out_msg_reject;
}

bool    Channel::GetM_Flag( void )
{
    return this->moderated_channel;
}

int     Channel::GetL_Flag( void )
{
    return this->channel_max_users;
}

void    Channel::SetP_Flag( bool P )
{
    this->private_channel = P;
}

void    Channel::SetS_Flag( bool S )
{
    this->secret_channel = S;
}

void    Channel::SetI_Flag( bool I )
{
    this->invite_only = I;
}

void    Channel::SetT_Flag( bool T )
{
    this->topic_opr_only = T;
}

void    Channel::SetN_Flag( bool N )
{
    this->out_msg_reject = N;
}

void    Channel::SetM_Flag( bool M )
{
    this->moderated_channel = M;
}

void    Channel::SetL_Flag( int max )
{
    this->channel_max_users = max;
}

void Channel::AddInvitedClient ( int client_sd )
{
    for (size_t it = 0; it < invited_clients_sd.size(); it++)
    {
        if (invited_clients_sd[it] == client_sd)
            return ;
    }
    invited_clients_sd.push_back(client_sd);
}
