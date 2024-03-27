/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 16:28:08 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/27 21:38:39 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <unistd.h> // For read
# include <exception>
# include <iostream>
# include <string>

# define PORT 4242
# define MAX_Q 10
# define BUFSIZE 3000

class Socket
{
private:
	int	_socket_fd;
	struct sockaddr_in _client_addr;
	struct sockaddr_in _server_addr;
	size_t _addr_size;
	
public:
	Socket(); //build the socket, bind and listen -> what if client ?
	~Socket(); //shutdown and close the socket
	Socket(const Socket& src);
	Socket& operator=(const Socket& src);
	
	int	_init_serversock(); // use later on to clear all sockaddr_in, set to 0 before copy or construct
	void	connection_handler();

	// keep some space for I/O Multiplexing lateron
};


#endif