/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 13:31:30 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 21:28:06 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Socket.hpp>
#include <errno.h>

Socket::Socket( void ) {
    return ;
}

Socket::~Socket( void ) {
    return ;
}

sockaddr_in    Socket::createSocket( const Directives &d ) {
    this->_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_sock_fd < 0)
        throw std::runtime_error("Error creating the socket");
    return (this->makeConnections(d, this->_sock_fd));
}

sockaddr_in	    Socket::makeConnections( const Directives &d, int sock_fd ) {
    sockaddr_in addr;
    std::string ip = d.getIp();
	addr.sin_family = AF_INET;
	addr.sin_port = htons(d.getPort());
	this->_addr_size = sizeof(const struct sockaddr_in);
    
	if (ip.empty() || !ip.compare("0.0.0.0"))
		addr.sin_addr.s_addr = INADDR_ANY;
	else
		inet_pton(addr.sin_family, ip.c_str(), &addr.sin_addr);
    
    if (bind(sock_fd, (const struct sockaddr *)&addr, this->_addr_size) < 0)
    {
        std::string error = strerror(errno);
        throw runtime_error("Socket binding the socket: " + error);
    }
    return (addr);
}

void             Socket::acceptConnections( void ) {
    this->_accepted_fd = accept(this->_sock_fd, (struct sockaddr *)&this->_client_addr, &this->_addr_size);
    if (this->_accepted_fd < 0)
    {
        std::string error = strerror(errno);
        throw runtime_error("Socket error: " + error);
    }
    fcntl(this->_accepted_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

void            Socket::listenConnections( void ) const {
    if (listen(this->_sock_fd, 0) < 0)
        return ;
}