/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 19:59:50 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/14 00:54:52 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EVENTS_HPP__
# define __EVENTS_HPP__

#include <sys/select.h>
#include <Logs.hpp>

class Events {
	private:
		Logs		_logs;
		fd_set		_readfds[1024];
		fd_set		_writefds[1024];
		fd_set		_exceptfds[1024];
		timeval		_timeout;
	public:
		Events( void );
		~Events( void );
		void	setReadFd( int fd );
		void	setWriteFd( int fd );
};

#endif