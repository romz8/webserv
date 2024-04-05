/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:28:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/05 10:27:40 by rjobert          ###   ########.fr       */
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
# include <errno.h>


# define MAX_Q 10
# define BUFSIZE 3000

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
	const int		acceptConnection();

	// keep some space for I/O Multiplexing lateron
};


void printSockAddrIn(const sockaddr_in& addr);

#endif