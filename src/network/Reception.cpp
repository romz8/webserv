/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 12:13:54 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/09 13:43:01 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Reception.hpp>
#include <Server.hpp>
#include <Events.hpp>
#include <Logs.hpp>

Reception::Reception( void ) {
	return ;
}

Reception::Reception( ServersVector &servers, Events *evs ) : _servers(servers), _evs(evs), timeout(1) {
	this->logs = Logs("reception_logs");
	if (this->_evs.isCreated == false)
	{
		this->logs.Error("Kqueue was not created correctly");
		throw std::runtime_error("kqueue error");
		return ;
	}
	setupServers();
	return ;
}

Reception::~Reception( void ) {
	return ;
}

void	Reception::setupServers( void ) {
	
}