/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 18:16:42 by rjobert           #+#    #+#             */
/*   Updated: 2024/05/28 15:05:30 by rjobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Cluster.hpp"

ServerConfig testBuild();
std::vector<ServerConfig> multipleTest();

int main(int argc, char *argv[])
{
	//ServerConfig test = testBuild();
    std::vector<ServerConfig> serverConfs = multipleTest();
	try
	{
        Cluster cluster(serverConfs);
        std::cout << BG_BLUE "BUFERSIZE: " << BUFSIZE << std::endl;
        cluster.run();
        //Server serv(test);
		// std::cout << "Server created" << std::endl;
		// std::cout << serv << std::endl;
		// serv.run();
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

ServerConfig testBuild(std::string hostname, int port)
{
	ServerConfig serverConfig;
    serverConfig.setHostName(hostname); 
    serverConfig.setRootDir("./html/");
    serverConfig.setPort(port);
    serverConfig.setClientMaxBodySize(100000000);
    serverConfig.setHost(serverConfig.getHostName() + ":" + std::to_string(serverConfig.getPort()));
    std::cout << "Host: " << serverConfig.getHost() << std::endl;
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
    location1.setUri("/test1");
    location1.setRoot("./html/getorder"); 
	location1.setAutoIndex(true);
    location1.setAllowedMethods(methodGP);
    location1.setIndex("index.html");
    location1.setAllowUpload(true);
    location1.setUploadDir("./upload/");
    serverConfig.addLocationConfig(location1);

    LocationConfig location2;
    location2.setUri("/test2");
    location2.setRoot(serverConfig.getRootDir() + "/postfile");
    location2.setAlias("https://www.youtube.com/watch?v=qoeCvcE-gKY");
    location2.setAllowedMethods(methodGP);
    //location2.setCgiPath("./upload/files/");
	location2.setAllowUpload(true);
    serverConfig.addLocationConfig(location2);
    std::cout << BLUE "Location 2: " << location2.getUri() << RESET << std::endl;

    LocationConfig Video;
    Video.setUri("/quesadillaVideo");
    Video.setAlias("https://www.youtube.com/watch?v=qoeCvcE-gKY");
    serverConfig.addLocationConfig(Video);

    LocationConfig recipe;
    recipe.setUri("/quesadillaRecipe");
    recipe.setAlias("https://www.simplyrecipes.com/recipes/quesadilla/");
    serverConfig.addLocationConfig(recipe);

    LocationConfig rootLocation;
    rootLocation.setUri("/");
    rootLocation.setRoot(serverConfig.getRootDir());
	rootLocation.setIndex("index.html");
    //rootLocation.setAlias("index.html");
    rootLocation.setAllowedMethods(methodGP);
    //rootLocation.setCgiPath("");
    rootLocation.addCgiConfig(CgiConfig(".sh", "/bin/bash"));
    rootLocation.addCgiConfig(CgiConfig(".py", "/usr/bin/python3"));
    rootLocation.addCgiConfig(CgiConfig(".js", "/usr/local/bin/node"));
    rootLocation.setErrorPages(serverConfig.getErrorPages());
    rootLocation.setAllowUpload(true);
    rootLocation.setUploadDir("/upload/");
    rootLocation.setAutoIndex(true);
    serverConfig.addLocationConfig(rootLocation);

	
	
	return serverConfig;
}

ServerConfig createServerConfig(std::string hostname, int port, const std::string& serverName) 
{
    ServerConfig serverConfig;
    serverConfig.setHostName(hostname);
    serverConfig.setPort(port);
    serverConfig.setServerName(serverName);
    serverConfig.setRootDir("./html/");
    serverConfig.addErrorPage(404, "error_pages/404.html");
    serverConfig.addErrorPage(500, "error_pages/500.html");
    serverConfig.addErrorPage(400, "error_pages/400.html");
    serverConfig.addErrorPage(413, "error_pages/413.html");

    // Add CGI configurations
    serverConfig.addCgiConfig(CgiConfig(".sh", "/bin/bash"));
    serverConfig.addCgiConfig(CgiConfig(".py", "/usr/bin/python3"));
    serverConfig.addCgiConfig(CgiConfig(".js", "/usr/local/bin/node"));

    std::vector<std::string> methods;
    methods.push_back("GET");
    methods.push_back("POST");

    // Create and configure LocationConfig instances
    LocationConfig location1;
    location1.setUri("/test1");
    location1.setRoot(serverConfig.getRootDir());
    location1.setAlias("index.html");
    location1.setAllowedMethods(methods);
    serverConfig.addLocationConfig(location1);

    LocationConfig location2;
    location2.setUri("/test2");
    location2.setRoot(serverConfig.getRootDir());
    location2.setAlias("index.html");
    location2.setAllowedMethods(methods);
    serverConfig.addLocationConfig(location2);

    return serverConfig;
}

std::vector<ServerConfig> multipleTest()
{
    std::vector<ServerConfig> serverConfs;
    //ServerConfig basic4243 = testBuild("127.0.0.1", 4243);
    ServerConfig basic4242 = testBuild("localhost", 4242);
    //ServerConfig basic4244 = testBuild("0.0.0.0", 4244);
    //serverConfs.push_back(basic4243);
    serverConfs.push_back(basic4242);
    //serverConfs.push_back(basic4244);
    return serverConfs;
}