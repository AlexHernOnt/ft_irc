/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 14:59:00 by ahernand          #+#    #+#             */
/*   Updated: 2023/05/06 17:29:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server.hpp"

void Server::Command_user( int client_sd, std::string data )
{
    std::cout << "ENTRA AL USER EL CLIENTE: " << client_list[client_sd].nick << " CON LOS SIGUIENTES DATOS: " << data << std::endl;

    //comprobar todo y luego sacar welcome si ok
    WelcomeClient(client_sd);
}

void Server::WelcomeClient( int client_sd )
{
	ServerMsgToClient(client_sd, "001", "Welcome to the Internet Relay Network " + client_list[client_sd].username + "!~" + client_list[client_sd].nick + "@" + client_list[client_sd].ip_address);

	/*
	:ircnet.hostsailor.com 001 albatrosstrosst :Welcome to the Internet Relay Network albatrosstrosst!~albatross@195.55.210.171
	:ircnet.hostsailor.com 002 albatrosstrosst :Your host is ircnet.hostsailor.com, running version 2.11.2p3+0PNv1.06
	:ircnet.hostsailor.com 003 albatrosstrosst :This server was created Thu May 20 2021 at 17:13:24 EDT
	:ircnet.hostsailor.com 004 albatrosstrosst ircnet.hostsailor.com 2.11.2p3+0PNv1.06 aoOirw abeiIklmnoOpqrRstv
	:ircnet.hostsailor.com 005 albatrosstrosst RFC2812 PREFIX=(ov)@+ CHANTYPES=#&!+ MODES=3 CHANLIMIT=#&!+:42 NICKLEN=15 TOPICLEN=255 KICKLEN=255 MAXLIST=beIR:64 CHANNELLEN=50 IDCHAN=!:5 CHANMODES=beIR,k,l,imnpstaqrzZ :are supported by this server
	:ircnet.hostsailor.com 005 albatrosstrosst PENALTY FNC EXCEPTS=e INVEX=I CASEMAPPING=ascii NETWORK=IRCnet :are supported by this server
	:ircnet.hostsailor.com 042 albatrosstrosst 0PNHAK5CQ :your unique ID
	:ircnet.hostsailor.com 251 albatrosstrosst :There are 19755 users and 2 services on 26 servers
	:ircnet.hostsailor.com 252 albatrosstrosst 65 :operators online
	:ircnet.hostsailor.com 253 albatrosstrosst 8 :unknown connections
	:ircnet.hostsailor.com 254 albatrosstrosst 11304 :channels formed
	:ircnet.hostsailor.com 255 albatrosstrosst :I have 753 users, 0 services and 1 servers
	:ircnet.hostsailor.com 265 albatrosstrosst 753 1328 :Current local users 753, max 1328
	:ircnet.hostsailor.com 266 albatrosstrosst 19755 25625 :Current global users 19755, max 25625
	:ircnet.hostsailor.com 375 albatrosstrosst :- ircnet.hostsailor.com Message of the Day -
	:ircnet.hostsailor.com 372 albatrosstrosst :- 9/5/2023 16:44
		... (todos los que llevan 372 son lineas del message of the day)

	:ircnet.hostsailor.com 376 albatrosstrosst :End of MOTD command.
	:albatrosstrosst MODE albatrosstrosst :+i
	*/
}
