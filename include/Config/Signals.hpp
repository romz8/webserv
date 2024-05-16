/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:47:27 by jsebasti          #+#    #+#             */
/*   Updated: 2024/03/28 16:25:29 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SIGNALS_HPP__
# define __SIGNALS_HPP__

# include <Defines.hpp>
# include <csignal>

class	Signals
{
	private:
		Signals( void );
		~Signals( void );

	public:
		static bool isRunning;
		static void	signalHandler(int signum);
		static void setSignalHandler(void);
};

#endif