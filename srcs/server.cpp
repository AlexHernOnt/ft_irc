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


#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>			// close 
#include <arpa/inet.h>		// close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>		// FD_SET, FD_ISSET, FD_ZERO macros 
#include <iostream>
#include <map>

#define TRUE	1
#define FALSE	0
#define PORT	6667 
	 
struct server_data {
	server_data()
		: password("1234"), port(PORT), max_clients(30) {}
	server_data(std::string password, int port)
        : password(password), port(port) {}

	//server properties
  	std::string	password;
  	int			port;
	int			max_clients;
};

struct client_data{
	client_data()
		: name("Cliente") {}
	client_data(std::string newName)
        : name(newName) {}

  	std::string		name;
	bool			password_passed;
	
	//room?
	//password set?
};

















int main(int argc , char *argv[])  
{  
	server_data							server;
	int									master_socket;
	int									addrlen;
	std::map<int, client_data>			client_list;			//<fd , client_data>
	int									max_sd;
	struct sockaddr_in					address;





	/*
	**		Crear Server
	*/

	int									opt = TRUE;

	// New server info
	server = server_data();

	// Set of socket descriptors 
	fd_set readfds;  

	// Create a master socket 
	if ((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{  
		std::perror("socket failed");  
		exit(EXIT_FAILURE);  
	}  

	// Set master socket to allow multiple connections , 
	// This is just a good habit, it will work without this 
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0)
	{
		std::perror("setsockopt");  
		std::exit(EXIT_FAILURE);
	}

	// Type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Bind the socket to localhost port 6667
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);

	// Ttry to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}















	/*
	**		Incorporar Cliente
	*/

	int									activity;
	int									new_socket;
	const char							*message = "Server: Welcome client!\nPassword: ";
	int									sd;




	// Accept the incoming connection
	addrlen = sizeof(address);
	std::puts("Waiting for connections ...");

	while (1)
	{
		// Clear the socket set
		FD_ZERO(&readfds);

		// Add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		// Add child sockets to set
		for (std::map<int, client_data>::iterator i = client_list.begin(); i != client_list.end(); std::advance(i, 1))
		{
			// Socket descriptor
			sd = i->first;

			// If valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET( sd , &readfds);

			// Highest file descriptor number, need it for the select function
			if (sd > max_sd)
				max_sd = sd;
		}

		// Wait for an activity on one of the sockets , timeout is NULL,
		// So wait indefinitely
		activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno != EINTR))
		{
			std::printf("select error");
		}

		// If something happened on the master socket , 
		// then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds))  
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *) &address, (socklen_t*)&addrlen)) < 0)
			{  
				std::perror("accept");  
				exit(EXIT_FAILURE);  
			}  

			//inform user of socket number - used in send and receive commands 
			std::printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

			//send new connection greeting message
			if (send(new_socket, message, std::strlen(message), 0) != std::strlen(message))
			{
				std::perror("send");
			}

			std::puts("Welcome message sent successfully");

			//add new socket to array of sockets
			//if position is not taken
			if(client_list.find(new_socket) == client_list.end())
			{
				// Creates a user in the map.
				client_list.insert(std::pair<int, client_data>(new_socket, client_data("Client " + std::to_string(new_socket))));
				std::printf("Adding to list of sockets\n");

				break;
			}
		}












		/*
		**		Disconnect and read
		*/


		char								buffer[1025];			//data buffer of 1K
		int									valread;
		int									sd;

		//else its some IO operation on some other socket
		for (std::map<int, client_data>::iterator i = client_list.begin(); i != client_list.end(); std::advance(i, 1))
		{
			sd = i->first;

			if (FD_ISSET(sd , &readfds))
			{
				//Check if it was for closing
				if ((valread = read(sd, buffer, 1024)) == 0)
				{
					printf("Host disconnected.");

					//Close the socket and mark as 0 in list for reuse
					close (sd);
					client_list.erase(i);
				}
				else
				{
					//set the string terminating NULL byte on the end of the data read

					buffer[valread] = '\0';

					// if it has not passed the password it can't send shit dude.
					if (client_list.find(sd)->second.password_passed == false)
					{
						if (valread != 0)
							buffer[valread - 1] = '\0';

						if (std::strcmp(buffer, server.password.c_str()) == 0)
						{
							client_list.find(sd)->second.password_passed = true;
							send(sd, "This is the correct password! Please input your ass:\n", 53, 0);
						}
						else
							send(sd, "Sorry but this is not the correct password\nPassword: ", 54, 0);

					}
					else
					{
						// Send message to every client
						for (std::map<int, client_data>::iterator j = client_list.begin(); j != client_list.end(); std::advance(j, 1))
						{
							if (j->first != sd)
							{
								std::string message = client_list.find(sd)->second.name + ": " + buffer;
								const char *aux = message.c_str();
								send(j->first, aux ,strlen(aux), 0);
							}
						}
						//std::printf("%s", buffer);
					}

					std::fill_n(buffer, 1024, 0);
				}
			}
		}
	}
	return 0;
}
