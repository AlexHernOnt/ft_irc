/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/04/06 17:41:26 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream> //probablemente va fuera
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>



// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080

int main(int argc, char const* argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	const char* hello = "Hello from server";
  
	// Creating socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	
	
	
	
	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
 
 
 
 
 
 
 
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
  
	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	while (42)
	{
		valread = read(new_socket, buffer, 1024);
		printf("%s\n", buffer);


	}
	printf("A\n");




	
	
	
	
	send(new_socket, hello, strlen(hello), 0);
	printf("Message sent\n");
  



  
	// closing the connected socket
	close(new_socket);
	// closing the listening socket
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}




















































































































































































































































































































































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
