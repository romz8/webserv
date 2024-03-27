/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:42 by rjobert           #+#    #+#             */
/*   Updated: 2024/03/27 19:39:06 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

int main(int argc, char *argv[])
{
	
	try
	{
		Socket sock;
		while (42)
			sock.connection_handler();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}