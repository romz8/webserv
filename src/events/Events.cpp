/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 19:59:36 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/13 20:12:23 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Events.hpp>
#include <unistd.h>

Events::Events( void ) {
	FD_ZERO(this->_readfds);
	FD_ZERO(this->_writefds);
	FD_ZERO(this->_exceptfds);
	this->_timeout.tv_sec = 0;
	this->_timeout.tv_usec = 0;
}

Events::~Events( void ) {
	FD_ZERO(this->_readfds);
	FD_ZERO(this->_writefds);
	FD_ZERO(this->_exceptfds);
	this->_logs.Info("Closed fds");
	return ;
}

void	Events::setReadFd( int fd ) {
	if (fd < 0 || fd >= 1024)
		throw std::runtime_error("Invalid fd");
	FD_SET(fd, this->_readfds);
	if (!FD_ISSET(fd, this->_readfds))
		throw std::runtime_error("Cannot put a fd in read mode");
	this->_logs.Info("The fd " + std::to_string(fd) + " is ready to read.");
}

void	Events::setWriteFd( int fd ) {
	if (fd < 0 || fd >= 1024)
		throw std::runtime_error("Invalid fd");
	FD_SET(fd, this->_writefds);
	if (!FD_ISSET(fd, this->_writefds))
		throw std::runtime_error("Cannot put a fd in write mode");
	this->_logs.Info("The fd " + std::to_string(fd) + " is readey to write.");
}