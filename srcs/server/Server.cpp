/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 13:53:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/04 21:57:44 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const testConf& conf) : _serverName(conf.serverName), _servAddr(setServAddr(conf)), _sock(socketFactory(_servAddr))
{
	this->_host = conf.host;
	this->_port = conf.port;
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

void	Server::run()
{
	while(42)
	{
		this->handleConnection();
	}
	// {
	// 	int io_fd = this->_sock.acceptConnection();
	// 	std::string head = this->_sock.readData(io_fd);
	// 	Header header(head);
	// 	header.buildRequest();
	// 	// if (header.getMethod() == "POST")
	// 	// {
	// 	// 		std::string body = this->_sock.readData(io_fd);
	// 	// 		std::cout << "Bodyreceived : " << body << std::endl;
	// 	// }
	// 	// std::string response = this->getResponse();
	// 	close(io_fd);
	// }
}

void	Server::handleConnection()
{
	int io_fd = this->_sock.acceptConnection();
	std::string head = this->_sock.readData(io_fd);
	Header header(head);
	header.buildRequest();
	// if (header.getMethod() == "POST")
	// {
	// 		std::string body = this->_sock.readData(io_fd);
	// 		std::cout << "Bodyreceived : " << body << std::endl;
	// }
	// std::string response = this->getResponse();
	header.printHeader();
	Response resp(header); // later on build with location routing Response response(header, _locs);
	resp.buildResponse();
	std::string response = resp.getResponse();
	//std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	int byteSend = send(io_fd, response.c_str(), response.size(), 0);
	if (byteSend < 0)
		throw std::runtime_error("IMpossible send message to client");
	close(io_fd);
}

Socket Server::socketFactory(const sockaddr_in& addr) 
{
    try 
	{
        return Socket(addr);
    } 
	catch(const std::exception& e) 
	{
        std::cerr << this->_serverName;
		std::cerr << "Failed to create socket: " << e.what() << std::endl;
        throw;
    }
}
