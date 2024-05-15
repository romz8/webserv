/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 13:31:30 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/15 15:27:08 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Socket.hpp>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

Socket::Socket( void ) {
	return ;
}

Socket::~Socket( void ) {
	close(this->_sock_fd);
	return ;
}

sockaddr_in		Socket::createSocket( const Directives &d ) {
	this->_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sock_fd < 0)
		throw std::runtime_error("Error creating the socket");
	return (this->makeConnections(d));
}

sockaddr_in		Socket::makeConnections( const Directives &d ) {
	sockaddr_in addr;
	std::string ip = d.getIp();
	addr.sin_family = AF_INET;
	addr.sin_port = htons(d.getPort());
	this->_addr_size = sizeof(const struct sockaddr_in);
	
	if (ip.empty() || !ip.compare("0.0.0.0"))
		addr.sin_addr.s_addr = INADDR_ANY;
	else
		inet_pton(addr.sin_family, ip.c_str(), &addr.sin_addr);

	int enable = 1;
	if (setsockopt(this->_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		std::string catchsys = strerror(errno);
		throw std::runtime_error("Error setsockopt() on the socket" + catchsys);
	}
	if (bind(this->_sock_fd, (const struct sockaddr *)&addr, this->_addr_size) < 0)
	{
		std::string error = strerror(errno);
		throw runtime_error("Socket binding the socket: " + error);
	}
	return (addr);
}

void			Socket::acceptConnections( void ) const {
	socklen_t	addr_size = sizeof(sockaddr);
	int accepted_fd = accept(this->_sock_fd, (struct sockaddr *)&this->_client_addr, &addr_size);
	if (accepted_fd < 0)
	{
		close(this->_sock_fd);
		std::string error = strerror(errno);
		throw runtime_error("Socket error: " + error);
	}
	fcntl(accepted_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	std::clog << "Accepted connection with: " << inet_ntoa(this->_client_addr.sin_addr) << ":" << this->_client_addr.sin_port << std::endl;
}

void		Socket::listenConnections( void ) {
	if (listen(this->_sock_fd, 420) < 0)
		throw std::runtime_error(strerror(errno));
}

int			Socket::getFd( void ) const {
	return (this->_sock_fd);
}