/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 10:21:24 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/31 10:30:49 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SIGNAL_HPP__
# define __SIGNAL_HPP__

# include <signal.h>

class Signal
{
private:
	Signal(/* args */);
public:
	~Signal();
	static void	sigHandler(int sig);
	static void	reciveSigs( void );
	static bool Running;
};

#endif