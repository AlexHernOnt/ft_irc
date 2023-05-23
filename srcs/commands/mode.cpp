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
        std::map<std::string, Channel>::iterator channel = channels_list.find(split_inputs[1]);
        if (channel == channels_list.end())
        {
            oss << split_inputs[1] << " :No such channel";
            ServerMsgToClient(client_sd, "403", oss.str());
            oss.str("");
            oss.clear();
            return;
        }
        //mode channel
        if (split_inputs.size() == 2)
        {
            //TODO: get info
            oss << split_inputs[1] << " " << channel->second.GetChannelFlags();;
            ServerMsgToClient(client_sd, "324", oss.str());
            oss.str("");
            oss.clear();
            return;
        }
        else
        {
            //set info
            if (channel->second.GetIfClientOperator(client_sd) == false) //check if opr
            {
                oss << split_inputs[1] << " :You're not channel operator";
                ServerMsgToClient(client_sd, "482", oss.str());
                oss.str("");
                oss.clear();
                return;
            }
            //TODO
            /*
                o - give/take channel operator privileges; X
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
            //add flags
            int propertyIndex = 3;
            bool sign = true;
            for (unsigned long i = 0; i < split_inputs[2].size(); i++)
            {
                if (split_inputs[2][i] == '+')
                {
                    sign = true;
                }
                else if (split_inputs[2][i] == '-')
                {
                    sign = false;
                }
                else if (split_inputs[2][i] == 'o')
                {
                    int target_client_sd = GetClientSdByNick(split_inputs[propertyIndex]);
                    if (target_client_sd == -1)
                    {
                        oss << split_inputs[propertyIndex] << " :No such nick/channel";
                        ServerMsgToClient(client_sd, "401", oss.str());
                        oss.str("");
                        oss.clear();
                        propertyIndex++;
                        continue;
                    }
                    if (channels_list[split_inputs[1]].GetIfClientInChannel(target_client_sd) == false)
                    {
                        oss << split_inputs[propertyIndex] << " " << split_inputs[1] << " :They aren't on that channel";
                        ServerMsgToClient(client_sd, "441", oss.str());
                        oss.str("");
                        oss.clear();
                        propertyIndex++;
                        continue;
                    }
                    if (sign == true)
                    {
                        channels_list[split_inputs[1]].MakeOperator( target_client_sd );
                        oss << "MODE " << split_inputs[1] << " +o " << split_inputs[propertyIndex];
                        SendClientMsgToChannel(client_sd, data, split_inputs[1]);
                        oss.str("");
                        oss.clear();
                    }
                    else
                    {
                        channels_list[split_inputs[1]].UnmakeOperator( target_client_sd );
                        oss << "MODE " << split_inputs[1] << " -o " << split_inputs[propertyIndex];
                        SendClientMsgToChannel(client_sd, data, split_inputs[1]);
                        oss.str("");
                        oss.clear();
                    }
                    
                    propertyIndex++;
                }
                //faltan las flags
                else
                {
                    oss << split_inputs[2][i] << " :is unknown mode char to me";
                    ServerMsgToClient(client_sd, "472", oss.str());
                    oss.str("");
                    oss.clear();
                }
            }
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
            //add flags
            ProcessClientFlags(client_sd, split_inputs[2]);
        }
    }

    /*
        ERR_CHANOPRIVSNEEDED            ERR_NOSUCHNICK
        ERR_NOTONCHANNEL                ERR_KEYSET
        RPL_BANLIST                     RPL_ENDOFBANLIST
    */
}
