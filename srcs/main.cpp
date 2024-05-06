/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:42 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/06 16:50:48 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char *argv[])
{
	Config test;
	test.host = "0"; test.port = "4242"; test.serverName = "testing Server";
	//test.root = "/Users/rjobert/Desktop/42_cursus/webserv/proto/html"; 
	test.root = "./html/";
	test.index = "index.html"; test.hostName = "localhost:4242";
	test.errPageGlobal.insert(std::pair<int, std::string>(404,"error_pages/404.html"));
	test.errPageGlobal.insert(std::pair<int, std::string>(500, "error_pages/500.html"));
	test.errPageGlobal.insert(std::pair<int, std::string>(400, "error_pages/400.html"));
	
	test.cgiConf.push_back(CgiConfig(".sh", "/bin/bash"));
	test.cgiConf.push_back(CgiConfig(".py", "/usr/bin/python"));
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