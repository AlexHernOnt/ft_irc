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
	name = DEFAULT_NAME;
	password = DEFAULT_PASSWORD;
	port = DEFAULT_PORT;
	timeout = 180000;
	nfds = 1;

	command_map["JOIN"] = &Server::Command_join;
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
	int    new_sd = -1;
	int    compress_array = FALSE;
	int    close_conn;
	char   buffer[80];
	
	int    current_size = 0;

	while (true)
	{
		std::cout << "Waiting on poll()..." << std::endl;
		if ((rc = poll(fds, nfds, timeout)) < 0)
		{
			perror("poll() failed");
			break;
		}

		if (rc == 0)
		{
			std::cout << "  poll() timed out.  End program." << std::endl;
			break;
		}
		
		current_size = nfds;
		for (int i = 0; i < current_size; i++)
		{
			/* find POLLIN and determine whether it's the listening  */
			/* or the active connection.                             */
			if(fds[i].revents == 0)
				continue;

			/* If revents is not POLLIN, it's an unexpected result,  */
			/* log and end the server.                               */
			if(fds[i].revents != POLLIN)
			{
				std::cout << "  Error! revents = " << fds[i].revents << std::endl;
				return;
			}

			if (fds[i].fd == master_socket)
			{
				std::cout << "Listening socket is readable" << std::endl;

				/* Accept all incoming connections that are            */
				/* queued up on the listening socket before we         */
				/* loop back and call poll again.                      */
				do
				{
					/* Accept each incoming connection. If               */
					/* accept fails with EWOULDBLOCK, then we            */
					/* have accepted all of them. Any other              */
					/* failure on accept will cause us to end the        */
					/* server.                                           */
					new_sd = accept(master_socket, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("accept() failed");
							return;
						}
						break;
					}

					/* Add the new incoming connection to the            */
					/* pollfd structure                                  */
					std::cout << "New incoming connection - " << new_sd << std::endl;
					fds[nfds].fd = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;

					/* Loop back up and accept another incoming          */
					/* connection                                        */
				} while (new_sd != -1);
			}

			/* This is not the listening socket, therefore an        */
			/* existing connection must be readable                  */
			else
			{
				std::cout << "  Descriptor " << fds[i].fd << " is readable" << std::endl;
				close_conn = FALSE;
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */

				while (true)
				{
					/* Receive data on this connection until the         */
					/* recv fails with EWOULDBLOCK. If any other         */
					/* failure occurs, we will close the                 */
					/* connection.                                       */
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						if (errno != EWOULDBLOCK)
						{
						perror("  recv() failed");
						close_conn = TRUE;
						}
						break;
					}

					/*****************************************************/
					/* Check to see if the connection has been           */
					/* closed by the client                              */
					/*****************************************************/
					if (rc == 0)
					{
						printf("  Connection closed\n");
						close_conn = TRUE;
						break;
					}

					/*****************************************************/
					/* Data was received                                 */
					/*****************************************************/
					len = rc;
					printf("  %d bytes received\n", len);

					/*****************************************************/
					/* Echo the data back to the client                  */
					/*****************************************************/
					rc = send(fds[i].fd, buffer, len, 0);
					if (rc < 0)
					{
						perror("  send() failed");
						close_conn = TRUE;
						break;
					}

				}

				/*******************************************************/
				/* If the close_conn flag was turned on, we need       */
				/* to clean up this active connection. This            */
				/* clean up process includes removing the              */
				/* descriptor.                                         */
				/*******************************************************/
				if (close_conn)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}
			}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors              */

		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		if (compress_array)
		{
			compress_array = FALSE;
			for (int i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for (int j = i; j < nfds-1; j++)
					{
						fds[j].fd = fds[j+1].fd;
					}
					i--;
					nfds--;
				}
			}
		}

	} /* End of serving running.    */
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
