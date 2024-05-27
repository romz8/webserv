/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:36:45 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/27 17:44:30 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Parser.hpp>
#include <ServerConfig.hpp>

Parser::Parser( void ) {
}

Parser::Parser( int ac, char **av ) {
	init();
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

void	Parser::check_command( int ac, char **av ) {
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

std::vector<ServerConfig> &Parser::getConfig( std::vector<ServerConfig> &servers, std::vector<std::string> allowed_directives ) {
	this->_allowed_directives = allowed_directives;
	while (_content.length() > 0)
	{
		try
		{
			parseLine(servers);
		} catch ( std::exception &e ) {
			throw std::logic_error(e.what());
		}
	}
	return (servers);
}

std::vector<std::string>	Parser::getAD( void ) {
	return (this->_allowed_directives);
}

void	Parser::parseLine(std::vector<ServerConfig> &servers ) {
	std::string	head;
	std::string	body;
	std::string	name;
	int			type;
	int			dirtype;

	if ( (type = SUtils::get_pair(this->_content, head, body) ) != NOT_SEPARATOR )
	{
		name = head.substr(0, head.find_first_of(ISSPACE));
		dirtype = checkValidDirective(name);
		checkValidSeparator(name, type);
		if (dirtype == SERVER)
			parseServer(head, body, servers);
		// else if (dirtype == LOCATION)
		// 	parseLocation(head, body, servers, dirtype);
		// else
		// 	parseDirective(head, body, servers, dirtype);
	}
	else
		throw std::logic_error( "Unxpected \"}\" or end of file" );
}

int	Parser::checkValidDirective( std::string name ) {
	if (SUtils::easyfind< std::vector<std::string> >(_directives.begin(), _directives.end(), name) == -1)
		throw std::logic_error(name + " is not a valid directive");
	else if (SUtils::easyfind< std::vector<std::string> >(_allowed_directives.begin(), _allowed_directives.end(), name) == -1)
		throw std::logic_error(name + " is not valid for this block");
	
	if (SUtils::easyfind< std::vector<std::string> >(_simpleDirectives.begin(), _simpleDirectives.end(), name) >= 0)
		return (SIMPLE);
	return (SUtils::easyfind< std::vector<std::string> >(_complexDirectives.begin(), _complexDirectives.end(), name));
}

void	Parser::checkValidSeparator( std::string name, int type ) {
	static std::string	separators[ 2 ] = { ";", "{" };
	int					expectedSeparator;

	if ( SUtils::easyfind< std::vector<std::string> >( \
				_simpleDirectives.begin(), \
				_simpleDirectives.end(), name ) >= 0 )
		expectedSeparator = SEMICOLON;
	else
		expectedSeparator = BRACET;
	if ( type != expectedSeparator )
		throw std::logic_error( "\"" + name \
				+ "\" invalid separator for this directive, expected \"" \
				+ separators[ expectedSeparator - 1 ] + "\"" );
}

void	Parser::parseServer( std::string head, std::string body, std::vector<ServerConfig> &servers ) {
	ServerConfig server(body);
}

// void	Parser::parseDirective( std::string head, std::string body, ServerConfig &, int dirtype ) {
// 	int idx;
// 	std::string name;

// 	name = head.substr(0, head.find_first_of(ISSPACE));
// 	if (dirtype ==)
// }