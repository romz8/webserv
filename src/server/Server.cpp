/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 10:48:47 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 21:25:35 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server( void ) {
	this->_d = new Directives();
	this->_d->def_serv_name = "webserv";
	return ;
}

Server::~Server( void ) {
	if (this->_d != NULL)
		delete this->_d;
	this->_d = NULL;
	return ;
}

Server::Server( const Server & src ) {
	*this = src;
}

Server	&Server::operator=( const Server & src ) {
	if (this != &src)
	{
		this->_d = new Directives(*src._d, 1);
		this->_sock = src._sock;
		this->_servAddr = src._servAddr;
	}
	return (*this);
}

void	Server::init_sock( void ) {
	this->_servAddr = this->_sock.createSocket(*this->_d);
	this->_sock.acceptConnections();
}

void	Server::run( void ) const {
	this->_sock.listenConnections();
}