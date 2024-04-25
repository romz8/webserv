/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:28:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/25 23:23:38 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <unistd.h> // For read
# include <arpa/inet.h> //for getaddress() convert Ip to integer
# include <exception>
# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <errno.h>


# define MAX_Q 10
# define BUFSIZE 8194
# define MAX_HEADER_SIZE 8193

class Socket
{
private:
	int	_socket_fd;
	struct sockaddr_in _client_addr;
	size_t _addr_size;
	
public:
	Socket(const sockaddr_in& servAddr); //build the socket, bind and listen -> what if client ?
	~Socket(); //shutdown and close the socket
	Socket(const Socket& src);
	Socket& operator=(const Socket& src);
	
	void	_initSock(); // use later on to clear all sockaddr_in, set to 0 before copy or construct
	const std::string readData(const int io_socket);
	const std::string readHeader(const int io_socket);
	const std::string readBody(const int io_socket, const std::map<std::string, std::string>& header, const std::string& rawhead);
	const int		acceptConnection();
	std::string		readFixedLengthBody(int clientSocket, size_t contentLength, std::string& body);	
	std::string		readChunkEncodingBody(int clientSocket, std::string& body);

	// keep some space for I/O Multiplexing lateron
};


void printSockAddrIn(const sockaddr_in& addr);

#endif