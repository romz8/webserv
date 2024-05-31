/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 10:20:08 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/31 10:33:25 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Signal.hpp>
#include <iostream>

Signal::Signal(){}

Signal::~Signal(){
}

bool	Signal::Running = true;

void	Signal::sigHandler(int sig) {
	std::cout << "Sig catched " << std::endl;
	if (sig == SIGQUIT || sig == SIGINT)
		Signal::Running = false;
	return ;
}

void	Signal::reciveSigs( void ) {
	signal(SIGINT, sigHandler);
	signal(SIGQUIT, sigHandler);
	signal(SIGPIPE, sigHandler);
}
