/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 12:14:08 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/14 00:50:48 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RECEPTION_HPP__
# define __RECEPTION_HPP__

#include <TypeDefines.hpp>
#include <Logs.hpp>

class Server;
class Events;

class Reception {
	private:
		Logs				logs;
		ServersVector		_servers;
		Events				*_evs;
		// int					timeout;
		Reception( void );

	public:
		~Reception( void );
		Reception( ServersVector & , Events *evs);

		void	setupServers( void );
		void	main_loop( void );
};

#endif