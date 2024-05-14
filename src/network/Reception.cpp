/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 12:13:54 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/14 23:56:17 by jsebasti         ###   ########.fr       */
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
	_timeout.tv_sec = 1;
	_timeout.tv_usec = 0;
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
			_servers[i]->setlisten();
			this->_evs->setRfds(_servers[i]->getFd());
		}
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return ;
	}
}

void	Reception::main_loop( void ) {
	while (Signals::isRunning)
	{
		fd_set	*evs = this->_evs->getRfds();
		std::cout << evs << endl;
		int ret = select(_servers[_servers.size() - 1]->getFd() + 1 , this->_evs->getRfds(), this->_evs->getWfds(), this->_evs->getEfds(), &this->_timeout);
		switch (ret)
		{
			case 0:
				this->logs.Error("Timeout");
				break ;
			case -1:
				this->logs.Error("Something went wrong");
			default:
				for (size_t i = 0; i < _servers.size(); i++)
				{
					if (this->_evs->checkRead(_servers[i]->getFd()))
						_servers[i]->run();
				}
				break ;
		}
	}
}