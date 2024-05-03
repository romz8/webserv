/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverGetters.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 18:41:35 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/03 18:42:04 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

const UintStrMap::mapped_type		&Server::getErrorPage( unsigned int error_code ) const {
	return (this->directives->getErrorPage(error_code));
}

const Location						*Server::getLocation( unsigned int num ) const {
	return (this->directives->getLocation(num));
}

int									Server::getStrongCoincidence( string name ) const {
	if (this->directives->dirSet["server_name"] == false)
		return (-1);
	for (size_t i = 0; i < this->directives->server_names.size(); i++)
	{
		if (!name.compare(this->directives->server_names[i]))
			return (i);
	}
	return (-1);
}