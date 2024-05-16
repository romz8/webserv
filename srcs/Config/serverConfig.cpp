/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 10:48:47 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/16 20:39:11 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ServerConfig.hpp>
#include <Directives.hpp>
#include <Logs.hpp>
#include <Socket.hpp>

ServerConfig::ServerConfig( void ) {
	this->_logs = Logs("ServerConfig_logs");
	this->_d = new Directives();
	this->_d->def_serv_name = "webserv";
	return ;
}

ServerConfig::~ServerConfig( void ) {
	this->_logs.Info("Deleting ServerConfig");
	if (this->_d != NULL)
		delete this->_d;
	this->_d = NULL;
	this->_logs.Info("ServerConfig deleted");
	return ;
}

ServerConfig::ServerConfig( ServerConfig & src ) {
	*this = src;
}

ServerConfig	&ServerConfig::operator=( ServerConfig & src ) {
	if (this != &src)
	{
		this->_logs = src._logs;
		this->_d = new Directives(*src._d, 1);
		this->_sock = src._sock;
		this->_servAddr = src._servAddr;
	}
	return (*this);
}

void	ServerConfig::init_sock( void ) {
	this->_servAddr = this->_sock.createSocket(*this->_d);
}

void	ServerConfig::setlisten( void ) {
	this->_sock.listenConnections();
}

void	ServerConfig::run( void ) const {
	try {
		this->_sock.acceptConnections();
	}
	catch (std::runtime_error & e) {
		std::cout << e.what() << std::endl;
	}
}

int		ServerConfig::getFd( void ) const {
	return (_sock.getFd());
}