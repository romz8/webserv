/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 10:48:47 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 00:58:37 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server( void ) {
	this->directives = new Directives();
	this->directives->def_serv_name = "webserv";
	return ;
}

Server::~Server( void ) {
	if (this->directives != NULL)
		delete this->directives;
	this->directives = NULL;
	return ;
}

Server::Server( const Server & src ) {
	*this = src;
}

Server	&Server::operator=( const Server & src ) {
	if (this != &src)
		this->directives = new Directives(*src.directives, 1);
	return (*this);
}

void	Server::init_sock( void ) {
	this->sock_fd = Socket::createSocket(AF_INET, SOCK_STREAM, 0);
	this->sockAddr = Socket::makeConnections( *this->directives );
}
