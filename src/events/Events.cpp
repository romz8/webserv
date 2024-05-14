/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 19:59:36 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/14 23:52:25 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Events.hpp>
#include <unistd.h>

Events::Events( void ) {
	FD_ZERO(&this->_rfds);
	FD_ZERO(&this->_wfds);
	FD_ZERO(&this->_efds);
	this->_timeout.tv_sec = 1;
	this->_timeout.tv_usec = 0;
}

Events::~Events( void ) {
	FD_ZERO(&this->_rfds);
	FD_ZERO(&this->_wfds);
	FD_ZERO(&this->_efds);
	this->_logs.Info("Closed fds");
	return ;
}

void	Events::setRfds( int fd ) {
	if (fd < 0 || fd >= 1024)
		throw std::runtime_error("Invalid fd");
	FD_SET(fd, &this->_rfds);
	if (!FD_ISSET(fd, &this->_rfds))
		throw std::runtime_error("Cannot put a fd in read mode");
	this->_logs.Info("The fd " + std::to_string(fd) + " is ready to read.");
}

void	Events::setWfds( int fd ) {
	if (fd < 0 || fd >= 1024)
		throw std::runtime_error("Invalid fd");
	FD_SET(fd, &this->_wfds);
	if (!FD_ISSET(fd, &this->_wfds))
		throw std::runtime_error("Cannot put a fd in write mode");
	this->_logs.Info("The fd " + std::to_string(fd) + " is readey to write.");
}

void	Events::setEfds( int fd ) {
		if (fd < 0 || fd >= 1024)
		throw std::runtime_error("Invalid fd");
	FD_SET(fd, &this->_efds);
	if (!FD_ISSET(fd, &this->_efds))
		throw std::runtime_error("Cannot put a fd in write mode");
	this->_logs.Info("The fd " + std::to_string(fd) + " is readey to write.");
}

fd_set	*Events::getRfds( void ) {
	return (&this->_rfds);
}

fd_set	*Events::getWfds( void ) {
	return (&this->_wfds);
}

fd_set	*Events::getEfds( void ) {
	return (&this->_efds);
}

int		Events::checkRead( int fd ) {
	if (!FD_ISSET(fd, &this->_rfds))
		return (0);
	return (1);
}