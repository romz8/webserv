/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romainjobert <romainjobert@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:31:46 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/12 12:27:57 by romainjober      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# include "Socket.hpp"
# include "Location.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "colors.h"


typedef struct Config
{
	std::string	host;
	std::string	port; //need to atoi : careful on overflow in conf file parsing
	std::string	serverName;
	std::string	root;
	std::string	index;
	std::string	hostName;
	//Location	loc; 	//later on std::vector<Location> locs;
} Config;


class Server
{
private:
	std::string	_host;
	std::string	_port;
	std::string	_serverName;
	std::string	_hostName;
	//Location _locs; //will be a vector later-on if several routes inf conf file
	const sockaddr_in _servAddr;
	Socket _sock;
	
	Server(const Server& src);
	Server& operator=(const Server& src);

	Socket socketFactory(const sockaddr_in& addr);

public:
	Server(const Config& conf);
	~Server();
	
	void 	_initServ();
	std::string getRequest();
	std::string getResponse();
	void	run();
	void	handleConnection();
	
	static const sockaddr_in setServAddr(const Config& conf);
};

#endif