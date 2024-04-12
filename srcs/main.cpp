/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romainjobert <romainjobert@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:42 by rjobert           #+#    #+#             */
/*   Updated: 2024/04/12 12:56:57 by romainjober      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char *argv[])
{
	Config test;
	test.host = "0"; test.port = "4242"; test.serverName = "testing Server";
	test.root = "/"; test.index = "index.html"; test.hostName = "localhost:4242";
	
	try
	{
		Server serv(test);
		serv.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

// HERE THE BASE CONF FILE TO TEST
// server
// {
// 	listen	0:4242;

// 	location /
// 	{
// 		allow_methods	GET;
// 		autoindex		on;
// 		index			siege.html;
// 	}
// }