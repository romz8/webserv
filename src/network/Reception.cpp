/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 12:13:54 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/15 19:25:10 by jsebasti         ###   ########.fr       */
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
	size_t	size = _servers.size();
	while (Signals::isRunning)
	{
		fd_set	*rfds = this->_evs->getRfds();
		fd_set	orfds = *rfds;
		fd_set	*wfds = this->_evs->getWfds();
		fd_set	owfds = *wfds;

		int ret = select(this->_nfds + 1 , rfds, wfds, NULL, &this->_timeout);
		if (ret < 0)
			this->logs.Error("Select failed");
		else if (ret == 0)
		{
			FD_COPY(&orfds, rfds);
			FD_COPY(&owfds, wfds);
			this->logs.Error("Timeout");
		}
		else
		{
			for (size_t i = 0; i < size; i++)
			{
				Server	*server = _servers[i];
				int		fd = server->getFd();
				if (this->_evs->checkRead(fd))
				{
					this->logs.Info("Reading");
					try {
						server->run();
					} catch (std::exception &e) {
						_evs->setRfds(fd);
						this->logs.Error(e.what());
					}
				}
				else if (this->_evs->checkWrite(fd))
				{
					this->logs.Info("Writting");
					try {
						server->sendInfo();
					} catch (std::exception &e) {
						this->logs.Error(e.what());
					}
				}
				else
					this->_evs->setRfds(fd);
			}
		}
	}
}