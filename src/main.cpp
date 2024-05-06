/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 03:04:10 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 21:15:21 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Signals.hpp"
#include "Defines.hpp"
#include <iostream>

int		main(int ac, char **av)
{
	try
	{
		Parser parser(ac, av);
		Directives *d = parser.getDirectives();
		const Server *s = d->getServer(0);
		Signals::setSignalHandler();
		while (Signals::isRunning)
		{
			s->run();
		}
		std::cout << " ------- Deleting server -------- " << std::endl;
		delete s;
		std::cout << " ------- Deleting directives ---- " << std::endl;
		delete d;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (FAILURE);
	}
	return (SUCCESS);
}
