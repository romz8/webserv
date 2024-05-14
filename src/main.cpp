/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 03:04:10 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/14 17:25:38 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Parser.hpp>
#include <Signals.hpp>
#include <Defines.hpp>
#include <Logs.hpp>
#include <Events.hpp>
#include <iostream>
#include <Reception.hpp>

using namespace std;

int		main(int ac, char **av)
{
	Logs	mainlog("main_logs");
	Directives *d; 
	try
	{
		Parser parser(ac, av);
		d = parser.getDirectives();
		mainlog.Info("Directives created");
	}
	catch(const exception& e)
	{
		mainlog.Error(e.what());
		return (FAILURE);
	}
	Signals::setSignalHandler();
	Events	evs;
	Reception	reciver(d->getServers(), &evs);
	reciver.main_loop();
	delete d;
	return (SUCCESS);
}
