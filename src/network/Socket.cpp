/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 13:31:30 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/03 08:52:20 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Socket.hpp>

Socket::Socket( void ) {
    return ;
}

Socket::~Socket( void ) {
    return ;
}

socket_t    Socket::createSocket( int domain, int type, int protocol ) {
    this->sock_fd = socket(domain, type, protocol);
    if (this->sock_fd < 0)
        throw std::runtime_error("Error setting the socket for ")
}