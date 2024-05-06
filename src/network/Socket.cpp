/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 13:31:30 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 09:52:39 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Socket.hpp>

Socket::Socket( void ) {
    return ;
}

Socket::~Socket( void ) {
    return ;
}

int    Socket::createSocket( int domain, int type, int protocol ) {
    int sock_fd = socket(domain, type, protocol);
    if (sock_fd < 0)
        throw std::runtime_error("Error setting the socket for domain: " + std::to_string(domain));
    return (sock_fd);
}

sockaddr_in	    Socket::makeConnections( const Directives &d, int sock_fd ) {
    sockaddr_in addr;
    std::string ip = d.getIp();
	addr.sin_family = AF_INET;
	addr.sin_port = htons(d.getPort());
	
	if (ip.empty() || !ip.compare("0.0.0.0"))
		addr.sin_addr.s_addr = INADDR_ANY;
	else
		inet_pton(addr.sin_family, ip.c_str(), &addr.sin_addr);
    
    if (bind(sock_fd, (const struct sockaddr *)&addr, sizeof(const struct sockaddr_in)) < 0)
    {
        std::string error = strerror(errno);
        throw runtime_error("Socket binding the socket: " + error);
    }
    return (addr);
}