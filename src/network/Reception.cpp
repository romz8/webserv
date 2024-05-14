/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 12:13:54 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/14 17:03:04 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Reception.hpp>
#include <Signals.hpp>
#include <Server.hpp>
#include <Events.hpp>

Reception::Reception( void ) {
	return ;
}

Reception::Reception( ServersVector &servers, Events *evs ) : _servers(servers), _evs(evs) {
	this->logs = Logs("reception_logs");
	setupServers();
	return ;
}

Reception::~Reception( void ) {
	return ;
}

void	Reception::setupServers( void ) {
	try
	{
		for (size_t i = 0; i < this->_servers.size(); i++) {
			int fd = _servers[i]->setlisten();
			this->_evs->setReadFd(fd);
		}
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return ;
	}
}

void	Reception::main_loop( void ) {
	while (Signals::isRunning)
	{}
}