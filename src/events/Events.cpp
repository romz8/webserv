/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 19:59:36 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/09 12:38:15 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Events.hpp>
#include <unistd.h>

Events::Events( void ) {
	this->_logs = Logs("events_logs");
	this->kq = kqueue();
	this->_logs.Info("Kqueue created");
}

Events::~Events( void ) {
	close(kq);
	this->_logs.Info("Closed Kqueue");
	return ;
}

bool	Events::isCreated( void ) {
	return (this->kq != -1);
}                                                                                              