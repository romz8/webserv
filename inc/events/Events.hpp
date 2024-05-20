/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 19:59:50 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/15 18:50:26 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EVENTS_HPP__
# define __EVENTS_HPP__

#include <sys/select.h>
#include <Logs.hpp>

class Events {
	private:
		Logs		_logs;
		fd_set		_rfds;
		fd_set		_wfds;
		fd_set		_efds;
		timeval		_timeout;
	public:
		Events( void );
		~Events( void );
		void	setRfds( int fd );
		void	setWfds( int fd );
		void	setEfds( int fd );

		fd_set		*getRfds( void );
		fd_set		*getWfds( void );
		fd_set		*getEfds( void );

		int			checkRead( int fd );
		int			checkWrite( int fd );
};

#endif