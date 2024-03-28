/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:57:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/28 17:06:48 by rjobert          ###   ########.fr       */
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
	if (bind(this->_socket_fd, (const struct sockaddr *) &servAddr, _addr_size) < 0)
		throw std::runtime_error("Error binding the socket");
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

//accept, read and send rep -> later pass over to Request class / receive from Response for Parsing
void	Socket::connection_handler()
{
	std::cout << "********* waiting for new connection *********" << std::endl;
	
	int io_socket = accept(this->_socket_fd, (struct sockaddr *) &this->_client_addr, (socklen_t *) &this->_addr_size);
	if (io_socket < 0)
		throw std::runtime_error("Error accepting client request");
	
	char buffer[BUFSIZE];
	int byteRead = recv(io_socket, buffer, BUFSIZE, 0);
	if (byteRead == 0)
		std::cout << "Connection closed" << std::endl;
	else if (byteRead < 0)
		throw std::runtime_error("Impossible read message from client");
	else
		std::cout << "[SERVER SIDE] : Client message is : " << buffer << std::endl;
	
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 17\n\nHello New world!";
	int byteSend = send(io_socket, response.c_str(), response.size(), 0);
	if (byteSend < 0)
		throw std::runtime_error("IMpossible send message to client");
	close(io_socket);
	std::cout << "********* MESSAGE SENT / NEXT CONNECTION *********" << std::endl;
}

void	Socket::_initSock()
{
	this->_socket_fd = 0;
	//this->_client_addr = {0}; //because libft allowed isn't clear (and so is memset then)
}

void printSockAddrIn(const sockaddr_in& addr) {
    // Convert the IP address to a string
    char* ipStr = inet_ntoa(addr.sin_addr);

    // Convert the port number from network byte order to host byte order
    unsigned int port = ntohs(addr.sin_port);

    std::cout << "IP Address: " << ipStr << ", Port: " << port << std::endl;
}