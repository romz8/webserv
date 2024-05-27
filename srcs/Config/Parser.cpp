/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:36:45 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/27 07:02:27 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Parser.hpp>
#include <ServerConfig.hpp>

Parser::Parser( void ) {
}

Parser::Parser( int ac, char **av ) {
	try {
		this->check_command(ac, av);
		this->getInfo();
	}
	catch (std::exception &e) {
		std::cerr << "[ERROR]	";
		throw std::runtime_error(e.what());
	}
}

Parser::~Parser( void ) {
}

void	Parser::check_command(int ac, char **av) {
	if (ac > 2)
		throw std::invalid_argument("Invalid number of arguments;\nUssage: ./webserv [config_file] (if not provided, it will use default.conf)");
	this->_file = av[1] ? av[1] : "resources/default.conf";
}

const std::string &Parser::getFile( void ) {
	return (this->_file);
}

void	Parser::getInfo( void ) {
	std::ifstream	confFile;
	std::string		line;

	confFile.open(this->getFile().c_str());
	if (!confFile.is_open())
		throw std::logic_error("The file " + this->getFile() + " couldn't be opened");
	while ( std::getline(confFile, line))
	{
		SUtils::treat_comments(line);
		line = trim_right(line);
		this->_content += line + "\n";
	}
	this->_content = SUtils::trim(this->_content);
	confFile.close();
}

std::vector<ServerConfig> &Parser::getConfig( std::vector<ServerConfig> &servers ) {
	try
	{
		while (_content.length() > 0)
		{
			parseLine(servers);
		}
		return (servers);
	} catch ( std::exception &e ) {
		throw std::logic_error(e.what());
	}
}

void	Parser::parseLine(std::vector<ServerConfig> &servers ) {
	std::string	head;
	std::string	body;
	std::string	name;
	int			type;

	if ( (type = SUtils::get_pair(this->_content, head, body) ) != NOT_SEPARATOR )
	{
		name = head.substr(0, head.find_first_of(ISSPACE));
		// checkValidDirective(name);
	}
}

void	Parser::checkValidDirective( std::string name ) {
	
}