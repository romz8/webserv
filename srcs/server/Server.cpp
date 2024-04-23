/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 13:53:36 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/23 22:07:55 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const Config& conf) : _serverName(conf.serverName), _servAddr(setServAddr(conf)), _sock(socketFactory(_servAddr))
{
	this->_host = conf.host;
	this->_port = conf.port;
	this->_serverName = conf.serverName;
	this->_hostName = conf.hostName;
	this->_root = conf.root;
	this->_maxBodySize = 10000000; //to replace with config max body size
	
	std::vector<std::string> methods1;
	std::vector<std::string> methods2;
	methods1.push_back("GET");
	methods2.push_back("GET");
	methods2.push_back("POST");

	_locations.push_back(Location("/", methods1, _root, "index.html", true, ""));
    _locations.push_back(Location("/getorder", methods2, _root, "index.html", false, "./uploads"));
    _locations.push_back(Location("/postfile", methods2, _root, "index.html", false, "./uploads"));
    //_locations.Location("/delete", {"DELETE"}, serverRoot, "test.html", false);
	
	printSockAddrIn(_servAddr);
}

Server::~Server(){}

void	Server::_initServ()
{
	_host.clear();
	_port.clear();
	_serverName.clear();
}

const sockaddr_in Server::setServAddr(const Config& conf)
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
	// 	Request Request(head);
	// 	Request.buildRequest();
	// 	// if (Request.getMethod() == "POST")
	// 	// {
	// 	// 		std::string body = this->_sock.readData(io_fd);
	// 	// 		std::cout << "Bodyreceived : " << body << std::endl;
	// 	// }
	// 	// std::string response = this->getResponse();
	// 	close(io_fd);
	// }
}

/*
TO CHANGE AS SUBJECT REQUIRES TO MAINTAIN OPEN -> close when recv reutnr 0 ?
*/
void	Server::handleConnection()
{
	int io_fd = this->_sock.acceptConnection();
	std::string rawhead = this->_sock.readHeader(io_fd);
	std::string head = rawhead.substr(0, rawhead.find("\r\n\r\n") + 4);
	std::cout << "rawHead: " << rawhead << std::endl;
	Request request(head, _hostName, _maxBodySize); //to replace with config max body size
	request.printHeader();
	const Location* matchLoc = findLocationForRequest(request.getPath());
	if (matchLoc == NULL)
		request.setStatus(404);
	else
		request.setLocation(*matchLoc);
	std::cout << "Location found : " << matchLoc->getPath() << std::endl;
	
	if (request.hasBody())
	{
		std::string body = this->_sock.readBody(io_fd, request.getHeader(), rawhead);
		request.setBody(body);
	}
	
	std::cout << BG_GREEN "********* DONE READING : NOW Build Request ********* " RESET << std::endl;
	request.buildRequest();
	request.printRequest();
	Response resp(request); // later on build with location routing Response response(Request, _locs);
	resp.buildResponse();
	std::string response = resp.getResponse();
	//std::cout << BG_MAGENTA "Response is : " RESET << response << std::endl;
	int byteSend = send(io_fd, response.c_str(), response.size(), 0);
	if (byteSend < 0)
		throw std::runtime_error("IMpossible send message to client");
	close(io_fd);
	std::cout << BG_GREEN	<< "Response sent : ALL OK" RESET << std::endl;
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

const Location* Server::findLocationForRequest(const std::string& requestPath) const 
{
    for (size_t i = 0; i < _locations.size(); ++i) {
        if (requestPath.find(_locations[i].getPath()) == 0) {
            return &_locations[i];
        }
    }
    return NULL;
}
