/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:36:45 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/09 09:26:23 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Parser.hpp>
#include <ParseContent.hpp>
#include <ParseDirectives.hpp>

Parser::Parser( void ) {
}

Parser::Parser( int ac, char **av ) {
	try {
		this->check_command(ac, av);
	}
	catch (std::invalid_argument &e) {
		
		std::cerr << e.what() << std::endl;
	}
}

Parser::~Parser( void ) {
}

void	Parser::check_command(int ac, char **av) {
	if (ac > 2)
		throw std::invalid_argument("Invalid number of arguments;\nUssage: ./webserv [config_file] (if not provided, it will use default.conf)");
	this->file = av[1] ? av[1] : "resources/default.conf";
}

const std::string &Parser::getFile( void ) {
	return (this->file);
}

void	Parser::getInfo( void ) {
	std::ifstream	confFile;
	std::string		line;

	confFile.open(this->file.c_str());
	if (!confFile.is_open())
		throw std::logic_error("The file " + this->getFile() + " couldn't be opened");
	while ( std::getline(confFile, line))
		this->content += line + "\n";
	this->content = SUtils::trim(this->content);	
	confFile.close();
}

Directives	*Parser::getDirectives( void ) {
	try
	{
		this->getInfo();
		this->_d = Directives::parseDirectives(this->content);
		return (this->_d);
	} catch ( std::exception &e ) {
		throw std::logic_error(e.what());
	}
}