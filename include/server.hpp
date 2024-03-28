/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 11:31:46 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/28 17:05:57 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# include "Socket.hpp"
# include "Location.hpp"


typedef struct s_testConf
{
	std::string	host;
	std::string	port; //need to atoi : careful on overflow in conf file parsing
	std::string	serverName;
	Location	loc; 	//later on std::vector<Location> locs;
} testConf;


class Server
{
private:
	std::string	_host;
	std::string	_port;
	std::string	_serverName;
	Location _locs; //will be a vector later-on if several routes inf conf file
	const sockaddr_in _servAddr;
	Socket _sock;
	
	Server(const Server& src);
	Server& operator=(const Server& src);

	Socket socketFactory(const sockaddr_in& addr);

public:
	Server(const testConf& conf);
	~Server();
	
	void 	_initServ();
	std::string getRequest();
	std::string getResponse();
	void	handleConnection();
	
	static const sockaddr_in setServAddr(const testConf& conf);
};

#endif