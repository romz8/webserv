/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:57:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/18 19:47:20 by rjobert          ###   ########.fr       */
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
	std::cout << "********* Accepted new connection *********  " << std::endl;
	int io_socket = accept(this->_socket_fd, (struct sockaddr *) &this->_client_addr, (socklen_t *) &this->_addr_size);
	std::cout << "ID SOCK IS " << io_socket << std::endl;
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

const std::string Socket::readHeader(const int io_socket)
{
	int byteRead = 1;
	std::string rawRequest;
	size_t endheader = std::string::npos;
	
	while(byteRead > 0)
	{
		char buffer[BUFSIZE];
		byteRead = recv(io_socket, buffer, BUFSIZE - 1, 0);
		if (byteRead < 0)
			throw std::runtime_error("Impossible read message from client");
		if (byteRead == 0)
			throw std::runtime_error("Client closed connection");
		buffer[byteRead] = '\0';
		rawRequest.append(buffer, byteRead);
		endheader = rawRequest.find("\r\n\r\n");
		if (endheader != std::string::npos)
			break ;
	}
	if (endheader == std::string::npos)
		throw std::runtime_error("Header too long");
	//std::string headers = rawRequest.substr(0, endheader + 4);
	std::cout << "********* DONE RECEIVING HEADER DATA *********" << std::endl;
	return (rawRequest);
}

/* QUICK AND DIRTY*/
const std::string Socket::readBody(const int io_socket, const std::map<std::string, std::string>& header, const std::string& rawhead)
{
	std::string body = rawhead.substr(rawhead.find("\r\n\r\n") + 4);
	std::cout << "BODY IS : " << body << std::endl;
	if (header.find("Transfer-Encoding") != header.end() && header.find("Transfer-Encoding")->second == "chunked")
	{
		std::string chunk;
		while (true)
		{
			chunk = readData(io_socket);
			if (chunk.empty())
				break ;
			body.append(chunk);
		}
	}
	else if (header.find("Content-Length") != header.end())
	{
		size_t contentLength = atoi((header.find("Content-Length")->second).c_str());
		body = readFixedLengthBody(io_socket, contentLength, body);
	}
	else
		throw std::runtime_error("Error: No content length or chunked encoding");
	std::cout << "********* DONE READING BODY DATA *********" << std::endl;
	return (body);
}

std::string Socket::readFixedLengthBody(int clientSocket, size_t contentLength, std::string& body) {
    
    int bytesRead = 0;
    size_t totalRead = 0;
	std::cout << "Content length : " << contentLength << std::endl;
	std::cout << "Body size : " << body.size() << std::endl;
	std::cout << "Body is : " << body << std::endl;
	char buffer[contentLength + 1];
    while (body.size() < contentLength) 
	{
		std::cout << "ENTER THE LOOP \n";
		bytesRead = recv(clientSocket, buffer, contentLength  - body.size(), 0);
        std::cout << "READ OK \n";
		if (bytesRead > 0)
            totalRead += bytesRead;
        else if (bytesRead == 0)
            break; // Connection closed
        else
            throw std::runtime_error("Error reading from socket");
		std::cout << "Bytes read : " << bytesRead << std::endl;
		std::cout << "Total read : " << totalRead << std::endl;
		buffer[bytesRead] = '\0';
		body.append(buffer, bytesRead);
    }
    buffer[totalRead] = '\0';
    std::string result(buffer, totalRead);
    return (body.append(result));
}
// {
// 	int byteRead = 1;
// 	std::string rawRequest;
// 	size_t endheader = std::string::npos;
	
// 	while(byteRead > 0)
// 	{
// 		char buffer[BUFSIZE];
// 		byteRead = recv(io_socket, buffer, BUFSIZE - 1, 0);
// 		if (byteRead < 0)
// 			throw std::runtime_error("Impossible read message from client");
// 		if (byteRead == 0)
// 			throw std::runtime_error("Client closed connection");
// 		buffer[byteRead] = '\0';
// 		rawRequest.append(buffer, byteRead);
// 		endheader = rawRequest.find("\r\n\r\n");
// 		if (endheader != std::string::npos)
// 			break ;
// 	}
// 	if (endheader == std::string::npos)
// 		throw std::runtime_error("Header too long");
// 	std::string headers = rawRequest.substr(0, endheader + 4);
// 	std::cout << "********* DONE TRANSMITTING DATA *********" << std::endl;
// 	return (headers);
// }

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