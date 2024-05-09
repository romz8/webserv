/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 10:48:47 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/09 13:58:56 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <Directives.hpp>
#include <Logs.hpp>
#include <Socket.hpp>

Server::Server( void ) {
	this->_logs = Logs("server_logs");
	this->_d = new Directives();
	this->_d->def_serv_name = "webserv";
	return ;
}

Server::~Server( void ) {
	this->_logs.Info("Deleting server");
	if (this->_d != NULL)
		delete this->_d;
	this->_d = NULL;
	this->_logs.Info("Server deleted");
	return ;
}

Server::Server( Server & src ) {
	*this = src;
}

Server	&Server::operator=( Server & src ) {
	if (this != &src)
	{
		this->_logs = src._logs;
		this->_d = new Directives(*src._d, 1);
		this->_sock = src._sock;
		this->_servAddr = src._servAddr;
	}
	return (*this);
}

void	Server::init_sock( void ) {
	this->_servAddr = this->_sock.createSocket(*this->_d);
}

int		Server::setlisten( void ) {
	return(this->_sock.listenConnections());
}

void	Server::run( void ) const {
	try {
		this->_sock.acceptConnections();
	}
	catch (std::runtime_error & e) {
		std::cout << e.what() << std::endl;
	}
}