/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverGetters.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 18:41:35 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 20:00:24 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

const UintStrMap::mapped_type		&Server::getErrorPage( unsigned int error_code ) const {
	return (this->_d->getErrorPage(error_code));
}

const Location						*Server::getLocation( unsigned int num ) const {
	return (this->_d->getLocation(num));
}

int									Server::getStrongCoincidence( string name ) const {
	if (this->_d->dirSet["server_name"] == false)
		return (-1);
	for (size_t i = 0; i < this->_d->server_names.size(); i++)
	{
		if (!name.compare(this->_d->server_names[i]))
			return (i);
	}
	return (-1);
}