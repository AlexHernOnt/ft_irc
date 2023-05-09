void Server::CheckConnections( void )
{
	// Clear the socket set
	FD_ZERO(&readfds);

	// Add master socket to set
	FD_SET(master_socket, &readfds);
	max_sd = master_socket;

	// Add child sockets to set
	for (std::map<int, client_data>::iterator i_client = client_list.begin(); i_client != client_list.end(); std::advance(i_client, 1))
	{
		// Socket descriptor
		int client_sd = i_client->first;

		// If valid socket descriptor then add to read list
		if (client_sd > 0)
			FD_SET( client_sd , &readfds);

		// Highest file descriptor number, need it for the select function
		if (client_sd > max_sd)
			max_sd = client_sd;
	}

	// Wait for an activity on one of the sockets , timeout is NULL,
	// So wait indefinitely
	activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);

	if ((activity < 0) && (errno != EINTR))
	{
		std::cout << "select error";
	}

	// If something happened on the master socket , 
	// then its an incoming connection 
	if (FD_ISSET(master_socket, &readfds))  
	{
		int new_socket;
		int	addrlen;
		if ((new_socket = accept(master_socket,
				(struct sockaddr *) &address, (socklen_t*)&addrlen)) < 0)
		{  
			std::perror("accept");  
			exit(EXIT_FAILURE);  
		}  

		//inform user of socket number - used in send and receive commands 
		std::cout << "New connection , socket fd is " << new_socket << ", ip is : " << inet_ntoa(address.sin_addr) << ", port : " << ntohs(address.sin_port) << std::endl;

		//send new connection greeting message
		if (send(new_socket, "Server: Welcome client!\nPassword: ", 34, 0) != 34)
		{
			std::perror("send");
		}

		std::cout << "Welcome message sent successfully" << std::endl;

		//add new socket to array of sockets
		//if position is not taken or if it's disconnected
		if(client_list.find(new_socket) == client_list.end())
		{
			// Creates a user in the map.
			client_list.insert(std::pair<int, client_data>(new_socket, client_data("Client " + std::to_string(new_socket))));
			std::cout << "Adding to list of clients" << std::endl;
		}
	}
}

void Server::CheckOperations( void )
{
	char								buffer[1025];			//data buffer of 1K
	int									valread;
	int									client_sd;

	//else its some IO operation on some other socket
	for (std::map<int, client_data>::iterator i_client = client_list.begin(); i_client != client_list.end(); std::advance(i_client, 1))
	{
		client_sd = i_client->first;

		if (FD_ISSET(client_sd , &readfds))
		{
			//Check if it was for closing
			if ((valread = read(client_sd, buffer, 1024)) == 0)
			{
				std::cout << "Client \"" << i_client->second.name.c_str() << "\" disconnected." << std::endl; //Creo que esto es para los clientes? (sobretodo porque el host suele ser el que actua de servidor también)

				//Close the socket
				close (client_sd);
				i_client->second.disconnected = true;
			}
			else
			{
				//set the string terminating NULL byte on the end of the data read

				buffer[valread] = '\0';

				// if it has not passed the password it can't send shit dude.
				if (i_client->second.password_passed == false)
				{
					if (valread != 0)
						buffer[valread - 1] = '\0';

					std::string buff_str = buffer;
					if (password.compare(buff_str) == 0)
					{
						i_client->second.password_passed = true;
						send(client_sd, "This is the correct password! Please input your ass:\n", 53, 0);
					}
					else
						send(client_sd, "Sorry but this is not the correct password\nPassword: ", 54, 0);
				}
				else
				{
					std::string buffer_str = buffer;
					ProcessCommand(i_client->first, buffer_str);
					// Send message to every client
					/*for (std::map<int, client_data>::iterator j = client_list.begin(); j != client_list.end(); std::advance(j, 1))
					{
						if (j->first != client_sd)
						{
							std::string message = client_list.find(client_sd)->second.name + ": " + buffer;
							const char *aux = message.c_str();
							send(j->first, aux ,strlen(aux), 0);
						}
					}*/
					//std::cout << buffer;
				}

				std::fill_n(buffer, 1024, 0);
			}
		}
	}
}