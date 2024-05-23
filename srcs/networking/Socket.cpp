/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:57:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/15 19:32:55 by rjobert          ###   ########.fr       */
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
	setNonBlocking(this->_socket_fd);
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
		std::cout << "Error binding the socket" << std::endl;
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
		this->_client_addr = src._client_addr;
	}
	return (*this);
}

/***************** Member Methods *******************************/

const int Socket::acceptConnection()
{
	//std::cout << "********* Accepted new connection *********  " << std::endl;
	int io_socket = accept(this->_socket_fd, (struct sockaddr *) &this->_client_addr, (socklen_t *) &this->_addr_size);
	//std::cout << "ID SOCK IS " << io_socket << std::endl;
	if (io_socket < 0)
		throw std::runtime_error("Error accepting client request");
	return (io_socket);
}

//read raw Browser request
// const std::string Socket::readData(const int io_socket)
// {
// 	int byteRead = 1;
// 	std::string rawRequest;
// 	while(byteRead > 0)
// 	{
// 		char buffer[BUFSIZE];
// 		byteRead = recv(io_socket, buffer, BUFSIZE - 1, 0);
// 		if (byteRead < 0)
// 			throw std::runtime_error("Impossible read message from client");
// 		buffer[byteRead] = '\0';
// 		rawRequest.append(buffer, byteRead);
// 		if (rawRequest.find("\r\n\r\n") != std::string::npos)
// 			break ;
// 	}
// 	//std::cout << "********* DONE TRANSMITTING DATA *********" << std::endl;
// 	return (rawRequest);
// }

int	Socket::readHeader(const int io_socket, std::string &rawRequest)
{
	int byteRead = 1;
	char buffer[BUFSIZE];
	byteRead = recv(io_socket, buffer, BUFSIZE - 1, 0);
	if (byteRead < 0)
		return (-1);
	if (byteRead == 0)
		return (0);
	buffer[byteRead] = '\0';
	rawRequest.append(buffer, byteRead);
	return (1);
}


int	Socket::readBody(const int io_socket, const std::map<std::string, std::string>& header, const std::string& rawhead, std::string& body)
{
	body = rawhead.substr(rawhead.find("\r\n\r\n") + 4);

	if (header.find("Transfer-Encoding") != header.end() && header.find("Transfer-Encoding")->second == "chunked")
		return (readChunkEncodingBody(io_socket, body));
	else if (header.find("Content-Length") != header.end())
	{
		size_t contentLength = std::stol((header.find("Content-Length")->second).c_str());
		std::cout << "content length is : " << contentLength << std::endl;
		return (readFixedLengthBody(io_socket, contentLength, body));
	}
	else
		return (-1);
}
/*
initially a loop on recv with a purpose to have the content added to initilay partial
body to be reconstructed in the end, but issue is that with a test made in a script in python
you can actually send a content-length of 10000 and empty body -> infinte loop so currenlty set up 
for a fixed length body to be read
*/
int Socket::readFixedLengthBody(int clientSocket, size_t contentLength, std::string& body) 
{
    
    int bytesRead = 0;
    size_t totalRead = 0;
	
	std::vector<char> buffer(contentLength + 1); 
	time_t startTime = std::time(NULL);
    while (body.size() < contentLength) 
	{
		if (difftime(std::time(NULL), startTime) > _readTimeout)
			return (-2);
		bytesRead = recv(clientSocket, buffer.data(), contentLength  - body.size(), 0);
		if (bytesRead > 0)
            totalRead += bytesRead;
        else if (bytesRead == 0)
            return (0);
		else if (bytesRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
			continue;
        else
		{
            std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
			return (-1);
		}
		buffer[bytesRead] = '\0';
		body.append(buffer.data(), bytesRead);
    }
    buffer[totalRead] = '\0';
    std::string result(buffer.data(), totalRead);
	body.append(result);
    return (1);
}

int Socket::readChunkEncodingBody(int clientSocket, std::string& body) 
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
		{
			std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
			return (-1);
		}
		if (bytesRead == 0)
			return (0);
		buffer[bytesRead] = '\0';
		data.append(buffer, bytesRead);
		if (data.find("0\r\n\r\n") != std::string::npos)
			endChunk = true;
	}
	body.append(data);
	std::cout << "Bytes read : " << bytesRead << std::endl;
	return (1);
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

int Socket::getSocketFd() const
{
	return (this->_socket_fd);
}

void	setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Impossible to get flags");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) //why this syntax ?
		throw std::runtime_error("Impossible to set non blocking");
}