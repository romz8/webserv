/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 13:27:44 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 21:26:27 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SOCKET_HPP__
# define __SOCKET_HPP__

# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <errno.h>
# include <fcntl.h>

class Directives;

class Socket
{
private:
	struct sockaddr_in	_client_addr;
	int					_sock_fd;
	int					_accepted_fd;
	socklen_t			_addr_size;

public:
	Socket( void );
	~Socket( void );
	sockaddr_in	createSocket( const Directives & );
	sockaddr_in	makeConnections( const Directives & , int );
	void		acceptConnections( void );
	void		listenConnections( void ) const ;
};

#include <Directives.hpp>

#endif
