/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 10:48:47 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/03 18:42:51 by jsebasti         ###   ########.fr       */
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
	{
		if (this->directives)
			delete this->directives;
		this->directives = new Directives(*src.directives, 1);
	}
	return (*this);
}
