/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:48:33 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/14 00:52:11 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Signals.hpp"
# include <iostream>

bool Signals::isRunning = true;

void	Signals::signalHandler(int signum) {
	switch (signum)
	{
		case SIGINT:
			std::cout << "Caught signal SIGINT" << std::endl;
			Signals::isRunning = false;
			exit(0);
			break;
		case SIGQUIT:
			std::cout << "Caught signal SIGQUIT" << std::endl;
			Signals::isRunning = false;
			break;
	}
}

void	Signals::setSignalHandler(void) {
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
}