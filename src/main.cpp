/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 03:04:10 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/23 11:11:50 by jsebasti         ###   ########.fr       */
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
		for (int i = 300; i < 310; i++)
		{
			
			std::cout << s->getErrorPage(i) << std::endl;
		}
		const Location *l = s->getLocation(0);
		std::cout << " ----- Locations -----\n" << l->getErrorPage(301) << std::endl;
		std::cout << l->getUri() << std::endl;
		std::cout << " ------- Deleting location ------ " << std::endl;
		delete l;
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
	
	// while (Signals::isRunning)
	// {
	// 	Signals::setSignalHandler();
	// }
	return (SUCCESS);
}
