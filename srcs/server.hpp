#ifndef _SERVER
# define _SERVER

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

#define DEFAULT_PASSWORD	"1234" 
#define DEFAULT_PORT	    6667 
	 
struct client_data{
	client_data()
		: name("Cliente"), disconnected(false) {}
	client_data(std::string newName)
        : name(newName), disconnected(false) {}

  	std::string		name;
	bool			password_passed;
	
	//room?
	//password set?

	bool			disconnected;
};

class Server {
    public:
        Server( void );
        ~Server( void );
        Server( const Server& other );

        //setup
        void ServerSocketSetup( void );

        //main loop
        void CheckConnections( void );
        void CheckOperations( void );

        //functions
    private:
        //server properties
        std::string	                password;
        int			                port;
        int			                max_clients;

        //server data
        std::map<int, client_data>	client_list;    //<sd (socket descriptor) , client_data>
        struct sockaddr_in			address;
        int                         master_socket;

        // Set of socket descriptors 
	    fd_set                      readfds;
        int                         max_sd;
        int							activity;
};

#endif