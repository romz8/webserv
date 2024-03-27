/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:57:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/27 21:39:19 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket()
{
	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket_fd < 0)
		throw std::runtime_error("Error creating socket");
	
	this->_server_addr.sin_family = AF_INET; //how for ipv6 ?
	this->_server_addr.sin_port = htons(PORT); // later replace on by server config
	this->_server_addr.sin_addr.s_addr = INADDR_ANY; // how modular ?
	
	this->_addr_size = sizeof(struct sockaddr_in);
	
	if (bind(this->_socket_fd, (const struct sockaddr *) &this->_server_addr, _addr_size) < 0)
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
	*this = src;
}

Socket& Socket::operator=(const Socket& src)
{
	if (this != &src)
	{
		this->_socket_fd = src._socket_fd;
		this->_server_addr = src._server_addr;
		this->_addr_size = src._addr_size;
	}
	return (*this);
}

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
	
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	int byteSend = send(io_socket, response.c_str(), response.size(), 0);
	if (byteSend < 0)
		throw std::runtime_error("IMpossible send message to client");
	close(io_socket);
	std::cout << "********* MESSAGE SENT / NEXT CONNECTION *********" << std::endl;
}
