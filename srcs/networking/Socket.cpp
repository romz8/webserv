/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:57:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/05 11:03:21 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"


/***************** Canonicalls *******************************/
Socket::Socket(const sockaddr_in& servAddr)
{
	_initSock();
	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket_fd < 0)
		throw std::runtime_error("Error creating socket");
	this->_addr_size = sizeof(struct sockaddr_in);
	
	printSockAddrIn(servAddr);
	
	int enable = 1;
	if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		std::string catchsys = strerror(errno);
		throw std::runtime_error("Error setsockopt() on the socket" + catchsys);
	}
	if (bind(this->_socket_fd, (const struct sockaddr *) &servAddr, _addr_size) < 0)
	{
		std::string catchsys = strerror(errno);
		throw std::runtime_error("Error binding the socket" + catchsys);
	}
	if (listen(this->_socket_fd, MAX_Q) < 0)
		throw std::runtime_error("Error listening socket stage");
}


Socket::~Socket()
{
	close(this->_socket_fd);
}


Socket::Socket(const Socket& src)
{
	_initSock();
	*this = src;
}

Socket& Socket::operator=(const Socket& src)
{
	if (this != &src)
	{
		_initSock();
		this->_socket_fd = src._socket_fd;
		this->_addr_size = src._addr_size;
	}
	return (*this);
}

/***************** Member Methods *******************************/

const int Socket::acceptConnection()
{
	std::cout << "********* Accepted new connection *********" << std::endl;
	int io_socket = accept(this->_socket_fd, (struct sockaddr *) &this->_client_addr, (socklen_t *) &this->_addr_size);
	if (io_socket < 0)
		throw std::runtime_error("Error accepting client request");
	return (io_socket);
}

//read raw Browser request
const std::string Socket::readData(const int io_socket)
{
	int byteRead = 1;
	std::string rawRequest;
	while(byteRead > 0)
	{
		char buffer[BUFSIZE];
		byteRead = recv(io_socket, buffer, BUFSIZE - 1, 0);
		if (byteRead < 0)
			throw std::runtime_error("Impossible read message from client");
		buffer[byteRead] = '\0';
		rawRequest.append(buffer, byteRead);
		if (rawRequest.find("\r\n\r\n") != std::string::npos)
			break ;
	}
	std::cout << "********* DONE TRANSMITTING DATA *********" << std::endl;
	return (rawRequest);
}

void	Socket::_initSock()
{
	this->_socket_fd = 0;
	//this->_client_addr = {0}; //because libft allowed isn't clear (and so is memset then)
}

void printSockAddrIn(const sockaddr_in& addr) 
{
    // Convert the IP address to a string
    char* ipStr = inet_ntoa(addr.sin_addr);

    // Convert the port number from network byte order to host byte order
    unsigned int port = ntohs(addr.sin_port);

    std::cout << "IP Address: " << ipStr << ", Port: " << port << std::endl;
}