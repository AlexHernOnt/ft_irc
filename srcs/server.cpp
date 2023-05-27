/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgirondo <rgirondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/25 20:00:20 by rgirondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server( void )
{
	nfds = 1;

	command_map["PASS"] = &Server::Command_pass;
	command_map["NICK"] = &Server::Command_nick;
	command_map["USER"] = &Server::Command_user;

	command_map["OPER"] = &Server::Command_oper;

	command_map["JOIN"] = &Server::Command_join;
	command_map["PART"] = &Server::Command_part;
	command_map["LIST"] = &Server::Command_list;
	command_map["TOPIC"] = &Server::Command_topic;
	command_map["MODE"] = &Server::Command_mode;
	command_map["KICK"] = &Server::Command_kick;
	command_map["INVITE"] = &Server::Command_invite;

	command_map["PRIVMSG"] = &Server::Command_privmsg;
	command_map["NOTICE"] = &Server::Command_notice;

	command_map["QUIT"] = &Server::Command_quit;

	//command_map["EXIT"] = &Server::Command_EXIT;
}

Server::~Server( void )
{
}

Server::Server( const Server& other )
{
    *this = other;
}

void Server::SetServerData( std::string password, int port )
{
	this->password = password;
	this->port = port;
}

void Server::ServerSocketSetup( void )
{
	int rc, opt = 1;

	// Create a master socket 
	if ((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{  
		std::perror("socket() failed");  
		exit(EXIT_FAILURE);
	}  

	// Set master socket to allow multiple connections , 
	// This is just a good habit, it will work without this 
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		std::perror("setsockopt() failed");
		close(master_socket);  
		std::exit(EXIT_FAILURE);
	}

	//non-blocking socket (no espera a la respuesta)
	if ((rc = fcntl(master_socket, F_SETFL, O_NONBLOCK)) < 0)
	{
		perror("fcntl() failed");
		close(master_socket);
		exit(EXIT_FAILURE);
	}

	// Type of socket created
	address.sin_family = AF_INET;			//AF_INET = ipv4, AF_INET6 = ipv6
	address.sin_addr.s_addr = INADDR_ANY;	//IP (localhost, básicamente)
	address.sin_port = htons(port);			//puerto

	// Bind the socket to localhost + port
	if ((rc = bind(master_socket, (struct sockaddr *)&address, sizeof(address))) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Try to specify maximum of MAX_CLIENTS pending connections for the master socket
	if (listen(master_socket, MAX_CLIENTS) < 0)
	{
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}
	memset(fds, 0, sizeof(fds));
	fds[0].fd = master_socket;
	fds[0].events = POLLIN;

	std::cout << "Listener on port " << port << std::endl;
}

void Server::MainLoop( void )
{
	int    len, rc = 1;
	int    new_socket = -1;
	bool   compress_array = false;
	char   buffer[1024];
	std::string str_buffer;
	
	int    current_size = 0;

	while (true)
	{
		std::cout << "Waiting on poll()..." << std::endl;
		if ((rc = poll(fds, nfds, -1)) < 0)
		{
			perror("poll() failed");
			break;
		}

		if (rc == 0)
		{
			std::cout << "poll() timed out.  End program." << std::endl;
			break;
		}
		
		current_size = nfds;
		for (int i = 0; i < current_size; i++)
		{
			if(fds[i].revents == 0)
				continue;

			if(fds[i].revents != POLLIN)
			{
				/*std::cout << "Error: revents = " << fds[i].revents << std::endl;
				return;*/
				int client_sd = fds[i].fd;
				std::cout << "User " << client_list[client_sd].nick << " quitted unexpectedly." << std::endl << "Revents: " << fds[i].revents << std::endl;
				for (unsigned long i = 0; i < client_list[client_sd].channels_joined.size(); i++)
				{
					std::string channel_str = client_list[client_sd].channels_joined[i];
					channels_list[channel_str].PartClient( client_sd );
					channels_list[channel_str].RemoveInvited( client_sd );
				}
				client_list.erase(client_sd);
				close(client_sd);
				fds[i].fd = -1;
				compress_array = true;
				continue;
			}

			if (fds[i].fd == master_socket)
			{
				std::cout << "Listening socket is readable" << std::endl;
				do
				{
					int	addrlen;
					if ((new_socket = accept(master_socket, (struct sockaddr *) &address, (socklen_t*)&addrlen)) < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("accept() failed");
							return;
						}
						break;
					}
					client_list.insert(std::pair<int, client_data>(new_socket, client_data()));
					client_list[new_socket].ip_address = inet_ntoa(address.sin_addr);

					ServerMsgToClient( new_socket, "020", ":Please wait while we process your connection." );
					std::cout << "New incoming connection - " << client_list[new_socket].nick << std::endl;
					fds[nfds].fd = new_socket;
					fds[nfds].events = POLLIN;
					nfds++;
				} while (new_socket != -1);
			}
			else
			{
				std::cout << "Descriptor " << fds[i].fd << " is readable" << std::endl;
				close_conn = FALSE;

				while (true)
				{
					if ((len = recv(fds[i].fd, buffer, sizeof(buffer), 0)) < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("recv() failed");
							close_conn = TRUE;
						}
						break;
					}

					//no he conseguido que esto se dispare
					if (len == 0)
					{
						std::cout << "Connection closed" << std::endl;
						close_conn = TRUE;
						break;
					}

					std::cout << len << " bytes received" << std::endl;
					//lee y añade al buffer hasta que deje de llegar
					client_list[fds[i].fd].str_buffer += buffer;
					memset(buffer, 0, 1024);
				}
				//si el cliente tiene buffer
				if (client_list[fds[i].fd].str_buffer != "")
				{
					str_buffer = client_list[fds[i].fd].str_buffer;
					std::vector<std::string> buffer_lines = Split(str_buffer, "\n", "\r");
					std::cout << str_buffer << std::endl;
					if (str_buffer.back() != '\n' && str_buffer.back() != '\r')
					{
						client_list[fds[i].fd].str_buffer = buffer_lines.back();
						buffer_lines.pop_back();
					}
					else
						client_list[fds[i].fd].str_buffer = "";
					
					for (unsigned long k = 0; k < buffer_lines.size(); k++)
					{
						ProcessCommand(fds[i].fd, buffer_lines[k]);
					}
				}

				if (close_conn)
				{
					int client_sd = fds[i].fd;
					for (unsigned long i = 0; i < client_list[client_sd].channels_joined.size(); i++)
					{
						std::string channel_str = client_list[client_sd].channels_joined[i];
						channels_list[channel_str].PartClient( client_sd );
						channels_list[channel_str].RemoveInvited( client_sd );
					}
					client_list.erase(client_sd);
					close(client_sd);
					fds[i].fd = -1;
					compress_array = true;
				}
			}
		}

		if (compress_array)
		{
			compress_array = false;
			Compressfds();
		}
	}
}

void Server::Compressfds( void )
{
	for (int i = 0; i < nfds; i++)
	{
		if (fds[i].fd == -1)
		{
			for (int j = i; j < nfds - 1; j++)
			{
				fds[j].fd = fds[j + 1].fd;
			}
			i--;
			nfds--;
		}
	}
}

void Server::Cleanfds( void )
{
	for (int i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
		close(fds[i].fd);
	}
}

void Server::ProcessCommand( int client_sd, std::string line )
{
	int start = 0;
	if (line[0] == ':')
		start = line.find(' ') + 1;
	line = line.substr(start);
	std::string command = line.substr(0, line.find(' '));

	/*if (line[line.size() - 1] == '\r') //condicion en caso de que el cliente pase la linea con un \r al final
		line.pop_back();*/
	if (command_map.count(command))
	{
		CommFunct func = command_map[command];
		(this->*func)(client_sd, line);
	}
	//std::cout << "COMANDO: " << command << std::endl;
	//std::cout << "DATOS: " << line << std::endl;
}

void Server::ServerMsgToClient( int client_sd, std::string msgcode, std::string line )
{
	std::string formatted_msg = ":" + (std::string)HOST_NAME + " " + msgcode + " " + client_list[client_sd].nick + " " + line + "\n";
	const char* formatted_msg_char = formatted_msg.c_str();
	send(client_sd, formatted_msg_char, formatted_msg.size(), 0);
}

void Server::OtherMsgToClient( int client_sd, std::string line )
{
	const char* formatted_msg_char = line.c_str();
	send(client_sd, formatted_msg_char, line.size(), 0);
}

void Server::SendClientMsg( int client_sd, std::string line, int target_client_sd )
{
	std::string formatted_msg = ":" + client_list[client_sd].nick + "!~" + client_list[client_sd].username + "@" + client_list[client_sd].ip_address + " " + line + "\n";
	const char* formatted_msg_char = formatted_msg.c_str();
	send(target_client_sd, formatted_msg_char, formatted_msg.size(), 0);
}

void Server::SendClientMsgToChannel( int client_sd, std::string line, std::string channel_name, bool resendtoself )
{
	if (channels_list.find(channel_name) == channels_list.end())
		return;
	std::vector<int> channel_client_sd_list = channels_list.find(channel_name)->second.GetClients();
	for( unsigned long i = 0; i < channel_client_sd_list.size(); i++ )
	{
		if (resendtoself == false && channel_client_sd_list[i] == client_sd)
			continue;
		SendClientMsg( client_sd, line, channel_client_sd_list[i]);
	}
}

std::vector<std::string> Server::Split( std::string data, std::string delimiter, std::string delimiter2 )
{
	std::vector<std::string> split_inputs;

	std::string::size_type prev_pos = 0, pos = 0;

    while (true)
    {
		std::string::size_type pos3 = data.find(delimiter, pos);
		if (delimiter2 != "")
		{
			std::string::size_type pos2 = data.find(delimiter2, pos);
			if (pos2 < pos3)
				pos3 = pos2;
		}
		pos = pos3;
		std::string substring( data.substr(prev_pos, pos - prev_pos) );
		//std::cout << substring << " " << substring.size() << std::endl;
		if (substring != "")
        	split_inputs.push_back(substring);

		if (pos == std::string::npos)
			break;
        prev_pos = ++pos;
    }

	return split_inputs;
}

std::vector<std::string> Server::SplitMsg( std::string data )
{
	std::vector<std::string> split_inputs;

	std::string::size_type prev_pos = 0, pos = 0;

    pos = data.find(":", pos);
	std::string substring( data.substr(prev_pos, pos - prev_pos) );
	if (substring != "")
		split_inputs.push_back(substring);

	if (pos == std::string::npos)
		return split_inputs;
	prev_pos = ++pos;

	std::string substring2( data.substr(prev_pos) );
	if (substring2 != "")
		split_inputs.push_back(substring2);

	return split_inputs;
}

int Server::GetUnregisteredCount( void )
{
	int num_registered = 0;
	for (std::map<int, client_data>::iterator i_client = client_list.begin(); i_client != client_list.end(); std::advance(i_client, 1))
	{
		if (i_client->second.registered == false)
			num_registered++;
	}
	return num_registered;
}

int Server::GetOperatorCount( void )
{
	int num_oprtor = 0;
	for (std::map<int, client_data>::iterator i_client = client_list.begin(); i_client != client_list.end(); std::advance(i_client, 1))
	{
		if (i_client->second.oprtor == true)
			num_oprtor++;
	}
	return num_oprtor;
}

int Server::GetClientSdByNick( std::string nick )
{
	int user_sd = -1;
	for (std::map<int, client_data>::iterator i_client = client_list.begin(); i_client != client_list.end(); std::advance(i_client, 1))
	{
		if (i_client->second.nick == nick)
			return i_client->first;
	}
	return user_sd;
}

bool Server::CheckChannelName( std::string channel )
{
	if (channel[0] != '#' && channel[0] != '&')
		return false;
	for (unsigned long i = 0; i < channel.size(); i++)
    {
        if (channel[i] == 7 || channel[i] == ',')
            return false;
    }
	return true;
}

std::string Server::GetUserFlags( int client_sd )
{
	std::string response = "+";

	if (client_list[client_sd].invisible == true)
		response += "i";
	if (client_list[client_sd].serv_not_recv == true)
		response += "s";
	if (client_list[client_sd].recv_wallops == true)
		response += "w";
	if (client_list[client_sd].oprtor == true)
		response += "o";

	return response;
}

void Server::ProcessClientFlags( int client_sd, std::string flags )
{
	bool sign = true;
	bool error_sent = false;
	for (unsigned long i = 0; i < flags.size(); i++)
	{
		if (flags[i] == '+')
		{
			sign = true;
		}
		else if (flags[i] == '-')
		{
			sign = false;
		}
		else if (flags[i] == 'i')
		{
			client_list[client_sd].invisible = sign;
		}
		else if (flags[i] == 's')
		{
			client_list[client_sd].serv_not_recv = sign;
		}
		else if (flags[i] == 'w')
		{
			client_list[client_sd].recv_wallops = sign;
		}
		else if (flags[i] == 'o' && sign == false)
		{
			client_list[client_sd].oprtor = false;
		}
		else if (error_sent == false)
		{
			ServerMsgToClient(client_sd, "501", ":Unknown MODE flag");
			error_sent = true;
		}
	}
}
