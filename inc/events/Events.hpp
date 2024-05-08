/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 19:59:50 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/08 20:15:20 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EVENTS_HPP__
# define __EVENTS_HPP__

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

typedef struct kevent event;

class Events {
	private:
		int		kq;
	
	public:
		Events( void );
		~Events( void );
		bool	isCreated( void );
};

#endif