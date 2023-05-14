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
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/time.h>		// FD_SET, FD_ISSET, FD_ZERO macros 
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "channel.hpp"

#define TRUE	1
#define FALSE	0

#define HOST_NAME           "irc.ciudadreal.com"

#define DEFAULT_PASSWORD	"1234" 
#define DEFAULT_PORT	    6667 
#define MAX_CLIENTS         30
	 
struct client_data{
	client_data()
		: nick("*"), username(""), realname(""), password_passed(false), channel_name(""), oprtor(false), ip_address("127.0.0.1"), user_set(false), nick_set(false), registered(false) {}

  	std::string		nick;
    std::string     username;
    std::string     realname;

	bool			password_passed;

    //properties
    std::string     channel_name; //nombre del canal en el que está. "" si no está en ninguno
    bool            oprtor;

    std::string     ip_address;

    //state machine
    bool            user_set;
    bool            nick_set;

    bool            registered;
};

class Server {
    
    typedef void (Server::*CommFunct)(int, std::string);

    public:
        Server( void );
        ~Server( void );
        Server( const Server& other );

        //setup
        void ServerSocketSetup( void );

        //main loop
        void MainLoop( void );

        //end
        void Cleanfds( void );
        
        //functions
        void Compressfds( void );
        void ServerMsgToClient( int client_sd, std::string msgcode, std::string line );
        void WelcomeClient( int client_sd );

        //utils
        std::vector<std::string> Split( std::string data, std::string delimiter );
        int GetOperatorCount( void );
        int GetUnregisteredCount( void );
        int GetClientSdByNick( std::string nick );

        //commands
        void ProcessCommand( int client_sd, std::string line );

        //  connection with the server
        void Command_pass( int client_sd, std::string data );   //sets password
        void Command_nick( int client_sd, std::string data );   //sets nickname
        void Command_user( int client_sd, std::string data );   //sets user variables

        //  other commands
        void Command_oper( int client_sd, std::string data );   //ask for operator privileges
        void Command_quit( int client_sd, std::string data );   //end connection (client sent message example: "ERROR :Closing Link: 195.55.210.171 (Quit: potato)")

        //  channel commands
        void Command_join( int client_sd, std::string data );   //join a channel

        //  message commands
        void Command_privmsg( int client_sd, std::string data );    //mensaje para un canal (si empieza con #) o persona especifica
        void Command_notice( int client_sd, std::string data );     //mensaje que no espera respuesta
        
    private:
        //server properties
        std::string	                password;
        int			                port;

        //server data
        std::map<int, client_data>	client_list;    //<sd (socket descriptor) , client_data>
        struct sockaddr_in			address;
        int                         master_socket;

        // Set of socket descriptors 
        struct pollfd               fds[200];
        int                         nfds;

        // Channels
        std::map<std::string, Channel >  channels; // 1: nombre del canal 2: client_sd asociados

        //others
        int                              max_sd;
        int							     activity;
        std::map<std::string, CommFunct> command_map;
};

#endif