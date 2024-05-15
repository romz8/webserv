/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 12:13:54 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/15 15:28:54 by jsebasti         ###   ########.fr       */
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
			this->_nfds = _servers[i]->getFd();
			this->_evs->setRfds(this->_nfds);
		}
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return ;
	}
}

void	Reception::main_loop( void ) {
	this->logs.Info("Started main loop");
	while (Signals::isRunning)
	{
		int ret = select(this->_nfds + 1 , this->_evs->getRfds(), NULL, NULL, &this->_timeout);
		switch (ret)
		{
			case -1:
				this->logs.Error("Select failed");
				break ;
			case 0:
				this->logs.Error("Timeout");
				break ;
			default:
				for (size_t i = 0; i < _servers.size(); i++)
				{
					if (this->_evs->checkRead(_servers[i]->getFd()))
					{
						this->logs.Info("Reading");
						_servers[i]->run();
					}
				}
				break ;
		}
	}
}