/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/04/29 16:45:31 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux

#include <stdio.h> 
#include <string.h>			//strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>			//close 
#include <arpa/inet.h>		//close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>		//FD_SET, FD_ISSET, FD_ZERO macros 
#include <iostream>
#include <map>

#define TRUE	1
#define FALSE	0
#define PORT	8888 
	 
struct server_data {
	server_data()
		: password(""), port(8888) {}
	server_data(std::string password, int port)
        : password(password), port(port) {}

  	std::string password;
  	int port;
};

struct client_data {
	client_data()
		: name("Cliente") {}
	client_data(std::string newName)
        : name(newName) {}

  	std::string name;
};

int main(int argc , char *argv[])  
{  
	server_data					server;
	int							opt = TRUE;
	int							master_socket;
	int							addrlen;
	int							new_socket;
	int							client_socket[30];
	std::map<int, client_data>	client_list;	//<socket_fd , client_data>
	int							max_clients = 30;
	int							activity;
	int							i;
	int							valread;
	int							sd;
	int							max_sd;
	struct sockaddr_in			address;

	char						buffer[1025];  //data buffer of 1K 
	const char					*message = "Server: Welcome client!\n";  



	//new server info
	server = server_data();

	//set of socket descriptors 
	fd_set readfds;  

	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++)  
	{  
		client_socket[i] = 0;  
	}

	//create a master socket 
	if ((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{  
		perror("socket failed");  
		exit(EXIT_FAILURE);  
	}  

	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");  
		exit(EXIT_FAILURE);  
	}

	//type of socket created 
	address.sin_family = AF_INET;  
	address.sin_addr.s_addr = INADDR_ANY;  
	address.sin_port = htons(PORT);  

	//bind the socket to localhost port 8888 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)  
	{  
		perror("bind failed");  
		exit(EXIT_FAILURE);  
	}  
	printf("Listener on port %d \n", PORT);  

	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0)  
	{  
		perror("listen");  
		exit(EXIT_FAILURE);  
	}  















	/*
	**		Incorporar Cliente
	*/

	//accept the incoming connection 
	addrlen = sizeof(address);  
	puts("Waiting for connections ...");  

	while(TRUE)  
	{  
		//clear the socket set
		FD_ZERO(&readfds);  
	 
		//add master socket to set 
		FD_SET(master_socket, &readfds);  
		max_sd = master_socket;  
			 
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++)  
		{  
			//socket descriptor 
			sd = client_socket[i];  
				 
			//if valid socket descriptor then add to read list 
			if(sd > 0)  
				FD_SET( sd , &readfds);  
				 
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd)  
				max_sd = sd;  
		}  

		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
	   
		if ((activity < 0) && (errno!=EINTR))  
		{  
			printf("select error");  
		}  
			 
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds))  
		{  
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
			{  
				perror("accept");  
				exit(EXIT_FAILURE);  
			}  

			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
			client_list.insert(std::pair<int, client_data>(new_socket, client_data("Client " + std::to_string(new_socket))));


			//send new connection greeting message
			if (send(new_socket, message, strlen(message), 0) != strlen(message))
			{
				perror("send");
			}

			puts("Welcome message sent successfully");

			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty 
				if( client_socket[i] == 0 )  
				{  
					client_socket[i] = new_socket;  
					printf("Adding to list of sockets as %d\n" , i);  

					break;  
				}
			}
		}


















		/*
		**		Disconnect and read
		*/

		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)  
		{  
			sd = client_socket[i];  
				 
			if (FD_ISSET( sd , &readfds))  
			{  
				//Check if it was for closing , and also read the 
				//incoming message 
				if ((valread = read( sd , buffer, 1024)) == 0)  
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
						  inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}

				//Echo back the message that came in
				else
				{
					//set the string  terminating NULL byte on the end
					//of the data read

					buffer[valread] = '\0';
                    for (int j = 0; j < max_clients; ++j)
					{
						if (client_socket[j] != sd)
						{
							std::string message = client_list.find(sd)->second.name + ": " + buffer;
							const char *aux = message.c_str();

							send(client_socket[j] , aux , strlen(aux), 0);
						}
					}
						
					printf("%s", buffer);
					std::fill_n(buffer, 1024, 0);

					// send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}
	}
	return 0;
}



















































































// Verson multi cliente beñat


// #include <iostream>
// #include <fstream> //probablemente va fuera
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>

// // Server side C/C++ program to demonstrate Socket
// // programming
// #include <netinet/in.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #include <sys/poll.h>

// #define PORT 8080

// int main(int argc, char const* argv[])
// {
// 	int						server_fd, new_socket, valread;
// 	struct sockaddr_in		address;
// 	int						opt = 1;
// 	int						addrlen = sizeof(address);
// 	char					buffer[1024];
// 	struct pollfd			pfds[1024];
// 	int						numOfConnections = 0;
// 	const char*				hello = "Server: Hello!\n";








// 	/*
// 	**	Setting Up Server
// 	*/


// 	// Creating socket
// 	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
// 	{
// 		perror("socket failed");
// 		exit(EXIT_FAILURE);
// 	}

// 	// Forcefully attaching socket to the port 8080
// 	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
// 	{
// 		perror("setsockopt");
// 		exit(EXIT_FAILURE);
// 	}

// 	address.sin_family = AF_INET;
// 	address.sin_addr.s_addr = INADDR_ANY;
// 	address.sin_port = htons(PORT);
  
// 	// Forcefully attaching socket to the port 8080
// 	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
// 	{
// 		perror("bind failed");
// 		exit(EXIT_FAILURE);
// 	}

// 	if (listen(server_fd, 3) < 0)
// 	{
// 		perror("listen");
// 		exit(EXIT_FAILURE);
// 	}








// 	/*
// 	**	Setting Up Pull
// 	*/


// 	for(int i = 0; i < 1024; i++)
// 	{
// 		pfds[i].fd = -1;
// 		pfds[i].events = POLLIN;
// 	}


// 	std::fill_n(buffer, 1024, 0);








// 	/*
// 	**	Server start
// 	*/


// 	while (42)
// 	{
// 		// Mirar si hay nueva conexion
// 		new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

// 		if (new_socket != -1)
// 		{
// 			for (int i = 0; i < 1024; i++)
// 			{
// 				if (pfds[i].fd == new_socket)
// 					break;
// 				if (pfds[i].fd == -1)
// 				{
// 					pfds[i].fd = new_socket;
// 					write(1, "New connection\n", 15);
// 					send(new_socket, hello, strlen(hello), 0);
// 					numOfConnections++;
// 					break;
// 				}
// 			}
// 		}
// 		poll(pfds, numOfConnections, -1);
// 		//comprobar todas las conexiones
// 		for (int i = 0; i < numOfConnections; i++)
// 		{
// 			if (pfds[i].fd == -1)
// 				break;
// 			if (pfds[i].revents & POLLIN)
// 			{
// 				valread = read(pfds[i].fd, buffer, 1024);
// 				if (valread > 0)
// 				{
// 					write(1, buffer, valread);
// 					std::fill_n(buffer, 1024, 0);
// 				}
// 				else
// 				{
// 					write(1, "stdin closed\n", 13);
// 				}
// 			}
// 		}
// 	}

// 	send(new_socket, hello, strlen(hello), 0);
// 	printf("Message sent\n");

// 	// closing the connected socket
// 	close(new_socket);
// 	// closing the listening socket
// 	shutdown(server_fd, SHUT_RDWR);
// 	return 0;
// }






















































































































































































































































































































































/*

int main(int argc, char **argv)
{
	int					sockfd;						
	struct				sockaddr_in serv_addr;		
	int					port = 194;					
	int					backlog = 15;				
	struct sockaddr_in	cli_addr;					
	socklen_t			cli_length;					
	char				*msg = NULL;				




	//Prepare server port
	serv_addr.sin_family = AF_INET;			
	serv_addr.sin_addr.s_addr = INADDR_ANY;	
	serv_addr.sin_port = htons(port);		




	// Creates the socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));



	if (listen(sockfd, backlog) != 0)
		return 1;

	printf("Do you know?\n");





	while (1)
	{
		if (accept(sockfd, (struct sockaddr*)&cli_addr, &cli_length) == 0)
		{
			write(1, "Nueva conexion", 15);
		}
	}





	if (sendto(sockfd, "Hola", 4, 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) != 0)
		return 1;
	if (recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr*)&cli_addr, &cli_length) != 0)
		return 1;

	if (close(sockfd) != 0)
		return 1;

	return 0;
}


*/














































/*
int main(int argc, char **argv)
{
	int					sockfd;							//socket
	struct				sockaddr_in serv_addr;			//info de la dirección IP y puerto de nuestro socket
	int					port = 194;						//puerto manual (194 es el puerto que he encontrado por ahi que usa IRC)
	int					backlog = 15;					//número de conexiones a la vez que puede tener el servidor (creo) se usa en el punto 3.-
	struct sockaddr_in	cli_addr;						//info de la dirección IP y puerto del cliente intentando conectarse a nuestro servidor. Se usa en el punto 4.-
	socklen_t			cli_length;						//longitud en bytes del cliente que llegue. Se usa en el punto 4.-
	char				*msg = NULL;					//variable utilizada para escribir el mensaje recibido






	//info para el punto 2.-
	serv_addr.sin_family = AF_INET;						//ipv4
	serv_addr.sin_addr.s_addr = INADDR_ANY;				//dirección ip
	serv_addr.sin_port = htons(port);					//puerto

	//1.- crear el teléfono que pueda llamar y recibir llamadas
	//0: family (AF_INET = ipv4 / AF_INET6 = ipv6)
	//1: type (SOCK_STREAM = stream socket (conexion constante como llamada) / SOCK_DGRAM = Datagram socket (se envia data como mensajes))
	//2: protocol (IPPROTO_TCP / IPPROTO_UDP) (poner 0 deja TCP en stream socket y UDP en datagram socket)
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	//2.- darle al teléfono la info como el número de teléfono que tiene
	//0: sockfd (socket descriptor devuelto por socket())
	//1: serv_addr (struct que contiene la info de la IP y puerto del servidor)
	//2: addrlen (longitud de dirección en bytes)
	bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	//connect() es utilizado por el cliente; por lo que no deberíamos usarlo nosotros tengo endendido
	//aunque no lo tengo del todo claro ya que parece que debería poder conectarse a la network
	//es básicamente bind en cliente

	//3.- decirle al teléfono que se prepare para poder recibir llamadas
	//0: sockfd (socket descriptor devuelto por socket())
	//1: backlog (número de conexiones a la vez que puede tener el servidor (creo))
	//devuelve si se ha realizado exitosamente o no
	if (listen(sockfd, backlog) != 0)
		return 1;
		
	//4.- aceptar aka coger el teléfono a una llamada
	//0: sockfd (socket descriptor devuelto por socket())
	//1: cli_addr (struct que contiene la info de la IP y puerto del cliente)
	//2: addrlen (longitud de dirección en bytes)
	//creo que esto tiene que ir en un bucle infinito
	while (1)
	{
		if (accept(sockfd, (struct sockaddr*)&cli_addr, &cli_length) == 0)
		{
			write(1, "Nueva conexion", 15);
		}
	}

	//5.- la info enviada y recibida
	//send y recv son de tcp (stream) y sendto y recvfrom para udp (data)
	//0: sockfd (socket descriptor devuelto por socket())
	//1: msg (el mensaje a enviar/recibir)
	//2: len (longitud del mensaje)
	//3: flags
	//4: to/from (un struct sockaddr con la info de quien o a quien se envía)
	//5: tolen/fromlen (longitud de dirección (sockaddr) en bytes)
	if (sendto(sockfd, "Hola", 4, 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) != 0)
		return 1;
	if (recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr*)&cli_addr, &cli_length) != 0)
		return 1;

	if (close(sockfd) != 0)
		return 1;

	return 0;
}
*/
