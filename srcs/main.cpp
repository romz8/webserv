/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romainjobert <romainjobert@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:42 by rjobert           #+#    #+#             */
/*   Updated: 2024/06/04 18:38:40 by romainjober      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <vector>
#include "Parser.hpp"
#include "Cluster.hpp"

int main(int argc, char *argv[])
{
	std::vector<ServerConfig> serverConfs;
    std::vector<int> ports;
	try
	{
        Parser parse(argc, argv);
        std::string content = parse.getContent();
        parse.getConfig(serverConfs, parse.getAD(), content);
        for (size_t i = 0; i < serverConfs.size(); i++)
            ports.push_back(serverConfs[i].getPort());
        for (size_t i = 0; i < ports.size(); i++)
        {
            for (size_t j = i + 1; j < ports.size(); j++)
                if (ports[i] == ports[j])
                    throw std::logic_error("Unexpected ports equals");
        }
		Cluster cluster(serverConfs);
        cluster.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
    return (0);
}
