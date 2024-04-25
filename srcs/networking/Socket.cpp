/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:57:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/25 23:40:40 by rjobert          ###   ########.fr       */
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
	std::cout << "ARRIVED IN HEADER READING" << std::endl;
	char buffer[BUFSIZE];
	byteRead = recv(io_socket, buffer, BUFSIZE - 1, 0);
	if (byteRead < 0)
		throw std::runtime_error("Impossible read message from client");
	if (byteRead == 0)
		throw std::runtime_error("Client closed connection in HEADER reading");
	buffer[byteRead] = '\0';
	rawRequest.append(buffer, byteRead);
	std::cout << "********* DONE RECEIVING HEADER DATA *********" << std::endl;
	return (rawRequest);
}


const std::string Socket::readBody(const int io_socket, const std::map<std::string, std::string>& header, const std::string& rawhead)
{
	std::string body = rawhead.substr(rawhead.find("\r\n\r\n") + 4);
	std::cout << "BODY from Header is : " << body << std::endl;
	if (header.find("Transfer-Encoding") != header.end() && header.find("Transfer-Encoding")->second == "chunked")
		readChunkEncodingBody(io_socket, body);
	else if (header.find("Content-Length") != header.end())
	{
		size_t contentLength = std::stol((header.find("Content-Length")->second).c_str());
		body = readFixedLengthBody(io_socket, contentLength, body);
	}
	else
		throw std::runtime_error("Error: No content length or chunked encoding");
	std::cout << "********* DONE READING BODY DATA *********" << std::endl;
	return (body);
}
/*
initially a loop on recv with a purpose to have the content added to initilay partial
body to be reconstructed in the end, but issue is that with a test made in a script in python
you can actually send a content-length of 10000 and empty body -> infinte loop so currenlty set up 
for a fixed length body to be read
*/
std::string Socket::readFixedLengthBody(int clientSocket, size_t contentLength, std::string& body) 
{
    
    int bytesRead = 0;
    size_t totalRead = 0;
	std::cout << "Content length : " << contentLength << std::endl;
	std::cout << "Body size : " << body.size() << std::endl;
	
	char buffer[contentLength + 1];
    while (body.size() < contentLength) 
	{
		std::cout << "total read : " << totalRead << std::endl;
		bytesRead = recv(clientSocket, buffer, contentLength  - body.size(), 0);
		if (bytesRead > 0)
            totalRead += bytesRead;
        else if (bytesRead == 0)
            break; // Connection closed
        else
            throw std::runtime_error("Error reading from socket");
		buffer[bytesRead] = '\0';
		body.append(buffer, bytesRead);
		if (totalRead == 0)
			throw std::runtime_error("Empty read");
		std::cout << "total read after first time : " << totalRead << std::endl;
    }
	std::cout << "Bytes read : " << bytesRead << std::endl;
	std::cout << "Total read : " << totalRead << std::endl;
    buffer[totalRead] = '\0';
    std::string result(buffer, totalRead);
	body.append(result);
    return (body);
}

std::string Socket::readChunkEncodingBody(int clientSocket, std::string& body) 
{
	std::string data;
	char buffer[BUFSIZE];
	int bytesRead = 0;
	bool endChunk = (body.find("0\r\n\r\n") != std::string::npos);
	
	data.clear();
	while(!endChunk)
	{
		bytesRead = recv(clientSocket, buffer, BUFSIZE - 1, 0);
		if (bytesRead < 0)
			throw std::runtime_error("Error reading from socket");
		if (bytesRead == 0)
		{
			throw std::runtime_error("readChunk : Connection closed by client ");
			break; // Connection closed
		}
		buffer[bytesRead] = '\0';
		data.append(buffer, bytesRead);
		if (data.find("0\r\n\r\n") != std::string::npos)
			endChunk = true;
	}
	body.append(data);
	std::cout << "Bytes read : " << bytesRead << std::endl;
	std::cout << "Data is : " << data << std::endl;
	std::cout << "Body is : " << body << std::endl;
	return (body);
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