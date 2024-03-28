/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 13:53:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/28 16:58:28 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const testConf& conf) : _servAddr(setServAddr(conf)), _sock(socketFactory(_servAddr))
{
	this->_host = conf.host;
	this->_port = conf.port;
	this->_serverName = conf.serverName;
	this->_locs = conf.loc;
	
	printSockAddrIn(_servAddr);
}

Server::~Server(){}

void	Server::_initServ()
{
	_host.clear();
	_port.clear();
	_serverName.clear();
}

const sockaddr_in Server::setServAddr(const testConf& conf)
{
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET; //how for ipv6 ?
	servAddr.sin_port = htons(atoi(conf.port.c_str())); // later replace on by server config
	
	//handling IP (either from 0 / nothing or "127.25.24") -> Will need to include getaddress()
	if (conf.host.empty() || conf.host == "0")
		servAddr.sin_addr.s_addr = INADDR_ANY;
	else
		inet_pton(servAddr.sin_family, conf.host.c_str(), &servAddr.sin_addr);
	std::cout << "CONF is host : " << conf.host << " | port : " << conf.port << std::endl;
	return (servAddr);
}

void	Server::handleConnection()
{
	while(42)
		this->_sock.connection_handler();
}

Socket Server::socketFactory(const sockaddr_in& addr) 
{
    try 
	{
        return Socket(addr);
    } catch(const std::exception& e) 
	{
        std::cerr << 
		std::cerr << "Failed to create socket: " << e.what() << std::endl;
        throw;
    }
}
