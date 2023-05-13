/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Server::Server( void )
{
	password = DEFAULT_PASSWORD;
	port = DEFAULT_PORT;
	nfds = 1;

	command_map["JOIN"] = &Server::Command_join;
	command_map["NICK"] = &Server::Command_nick;
	command_map["USER"] = &Server::Command_user;
}

Server::~Server( void )
{
}

Server::Server( const Server& other )
{
    *this = other;
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

	if ((rc = fcntl(master_socket, F_SETFL, O_NONBLOCK)) < 0)
	{
		perror("fcntl() failed");
		close(master_socket);
		exit(EXIT_FAILURE);
	}

	// Type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	// Bind the socket to localhost port 6667
	if ((rc = bind(master_socket, (struct sockaddr *)&address, sizeof(address))) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Ttry to specify maximum of MAX_CLIENTS pending connections for the master socket
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
	int    compress_array = FALSE;
	bool   close_conn;
	char   buffer[1024];
	
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
				std::cout << "User " << client_list[i].nick << " quitted unexpectedly." << std::endl << "Revents: " << fds[i].revents << std::endl;
				client_list.erase(fds[i].fd);
				close(fds[i].fd);
				fds[i].fd = -1;
				compress_array = TRUE;
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

					if (len == 0)
					{
						std::cout << "Connection closed" << std::endl;
						close_conn = TRUE;
						break;
					}

					std::cout << len << " bytes received" << std::endl;
					std::string str_buffer = buffer;
					ProcessCommand(fds[i].fd, buffer);
				}

				if (close_conn)
				{
					client_list.erase(fds[i].fd);
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}
			}
		}

		if (compress_array)
		{
			compress_array = FALSE;
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
	line[line.find('\n')] = '\0';
	std::string command = line.substr(0, line.find(' '));

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
