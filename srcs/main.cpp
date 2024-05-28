/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:42 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/28 16:33:51 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <vector>
#include "Parser.hpp"

ServerConfig testBuild();

int main(int argc, char *argv[])
{
	std::vector<ServerConfig> servs;
	ServerConfig test = testBuild();
	try
	{
        Parser parse(argc, argv);
        std::string content = parse.getContent();
        parse.getConfig(servs, parse.getAD(), content);
        return (1);
		Server serv(test);
		std::cout << "Server created" << std::endl;
		std::cout << serv << std::endl;
		serv.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

/*
Configuration class and setup corresponding to 
server {
    listen 4242;
    server_name testing Server;
    root ./html/;
    error_page 404 error_pages/404.html;
    error_page 500 error_pages/500.html;
    error_page 400 error_pages/400.html;
    error_page 413 error_pages/413.html;

    cgi .sh /bin/bash;
    cgi .py /usr/bin/python3;
    cgi .js /usr/local/bin/node;

    location /getorder {
        root ./html/;
        alias index.html;
        autoindex on;
        allow_methods GET POST;
    }

    location /postfile {
        root ./html/;
        alias index.html;
        allow_methods GET POST;
    }

    location / {
        root ./html/;
        index index.html;
        alias index.html;
        allow_methods GET POST;
        cgi .sh /bin/bash;
        cgi .py /usr/bin/python3;
        cgi .js /usr/local/bin/node;
        error_page 404 error_pages/404.html;
        error_page 500 error_pages/500.html;
        error_page 400 error_pages/400.html;
        error_page 413 error_pages/413.html;
    }
}
*/

ServerConfig testBuild()
{
	ServerConfig serverConfig;
	serverConfig.setHostName("127.0.0.1"); 
	serverConfig.setHost("127.0.0.1:4242"); //bad on purpose
    serverConfig.setRootDir("./html/");
    serverConfig.setPort(4242);
    serverConfig.setServerName("testing Server");
    serverConfig.addErrorPage(404, "error_pages/404.html");
    serverConfig.addErrorPage(500, "error_pages/500.html");
    serverConfig.addErrorPage(400, "error_pages/400.html");
    serverConfig.addErrorPage(413, "error_pages/413.html");

	// Add CGI configurations
    serverConfig.addCgiConfig(CgiConfig(".sh", "/bin/bash"));
    serverConfig.addCgiConfig(CgiConfig(".py", "/usr/bin/python3"));
    serverConfig.addCgiConfig(CgiConfig(".js", "/usr/local/bin/node"));

    std::vector<std::string> methodGP;
	std::vector<std::string> methodG;
	std::vector<std::string> methodD;
	methodGP.push_back("GET");
	methodGP.push_back("POST");
	methodG.push_back("GET");
	methodD.push_back("DELETE");
	
	// Create and configure LocationConfig instances
    LocationConfig location1;
    location1.setUri("/getorder");
    location1.setRoot(serverConfig.getRootDir());
    location1.setAlias("index.html"); // to implement
	location1.setAutoIndex(true);
    location1.setAllowedMethods(methodGP);
    serverConfig.addLocationConfig(location1);

    LocationConfig location2;
    location2.setUri("/postfile");
    location2.setRoot(serverConfig.getRootDir());
    location2.setAlias("index.html");
    location2.setAllowedMethods(methodGP);
    //location2.setCgiPath("./upload/files/");
	location2.setAllowUpload(true);
    serverConfig.addLocationConfig(location2);

    LocationConfig rootLocation;
    rootLocation.setUri("/");
    rootLocation.setRoot(serverConfig.getRootDir());
	rootLocation.setIndex("index.html");
    rootLocation.setAlias("index.html");
    rootLocation.setAllowedMethods(methodGP);
    //rootLocation.setCgiPath("");
    rootLocation.addCgiConfig(CgiConfig(".sh", "/bin/bash"));
    rootLocation.addCgiConfig(CgiConfig(".py", "/usr/bin/python3"));
    rootLocation.addCgiConfig(CgiConfig(".js", "/usr/local/bin/node"));
    rootLocation.setErrorPages(serverConfig.getErrorPages());
    serverConfig.addLocationConfig(rootLocation);

	
	
	return serverConfig;
}

