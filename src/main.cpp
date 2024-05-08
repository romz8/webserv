/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 03:04:10 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/08 20:02:29 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Parser.hpp>
#include <Signals.hpp>
#include <Defines.hpp>
#include <iostream>

int		main(int ac, char **av)
{
	Directives *d; 
	try
	{
		Parser parser(ac, av);
		d = parser.getDirectives();
		const Server *s = d->getServer(0);
		std::cout << " ------- Deleting server -------- " << std::endl;
		delete s;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (FAILURE);
	}
	Signals::setSignalHandler();
	while (Signals::isRunning)
	{
		d->setServersToListen();
		kqueue()
		d->run()
	}
	delete d;
	return (SUCCESS);
}
