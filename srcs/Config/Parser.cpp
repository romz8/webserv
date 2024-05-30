/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:36:45 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/30 15:41:55 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Parser.hpp>
#include <ServerConfig.hpp>
#include <ParseContent.hpp>
#include <LocationConfig.hpp>
#include <Utils.hpp>

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

StrVector	Parser::getAD( void ) {
	return (this->_allowed_directives);
}

void	Parser::checkValidDirective( std::string name, StrVector allowed_directives ) {
	if (SUtils::easyfind< StrVector >(_directives.begin(), _directives.end(), name) == -1)
		throw std::logic_error(name + " is not a valid directive");
	else if (SUtils::easyfind< StrVector >(allowed_directives.begin(), allowed_directives.end(), name) == -1)
		throw std::logic_error(name + " is not valid for this block");
	return ;
}

void	Parser::checkValidSeparator( std::string name, int type ) {
	static std::string	separators[ 2 ] = { ";", "{" };
	int					expectedSeparator;

	if ( SUtils::easyfind< StrVector >( \
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

//	SERVER FUNCTIONS -----------------------------

void	Parser::parseLine( std::vector<ServerConfig> &vector, StrVector allowed_directives, std::string &content) {
	std::string	head;
	std::string	body;
	std::string	name;
	int			type;

	if ( (type = SUtils::get_pair(content, head, body) ) != NOT_SEPARATOR )
	{
		name = head.substr(0, head.find_first_of(ISSPACE));
		checkValidDirective(name, allowed_directives);
		checkValidSeparator(name, type);
		parseDirective(head, body, vector, allowed_directives);
	}
	else
		throw std::logic_error( "Unxpected \"}\" or end of file" );
};

void	Parser::parseDirective( std::string head, std::string body, std::vector<ServerConfig> &server, StrVector allowed_directives ) {
	int idx;
	std::string name;

	name = head.substr(0, head.find_first_of(ISSPACE));
	if ((idx = SUtils::easyfind< StrVector >(_simpleDirectives.begin(), _simpleDirectives.end(), name)) >= 0)
		parseSimpleServ(head, server[server.size() - 1], allowed_directives);
	else
		parseComplex(head, body, server);
}

void Parser::getConfig( std::vector<ServerConfig> &vector, StrVector allowed_directives, std::string &content ) {
	while (content.length() > 0)
	{
		try
		{
			parseLine(vector, allowed_directives, content);
		} catch ( std::exception &e ) {
			throw std::logic_error(e.what());
		}
	}
}

void	Parser::parseSimpleServ( std::string head, ServerConfig &server, StrVector allowed_directives ) {
	std::string name = head.substr(0, head.find_first_of(ISSPACE));
	ParseContent::checkDuplicate<ServerConfig>(name, server);
	int idx = SUtils::easyfind< StrVector >(allowed_directives.begin(), allowed_directives.end(), name);
		(Parser::_parseSimpleSA[idx])(head, server);
}


void	Parser::parseComplex( std::string head, std::string body, std::vector<ServerConfig> &vector ) {
	std::string	name;
	
	name = head.substr(0, head.find_first_of(ISSPACE));
	if (!name.compare("server"))
		parseServer( body, vector);
	else
		parseLocation( head, body, vector);
}

void	Parser::parseServer( std::string body, std::vector<ServerConfig> &vector ) {
	ServerConfig server;
	vector.push_back(server);
	getConfig(vector, server.getAD(), body);
}


// 	LOCATIONS FUNCTIONS --------------------------------------------------------------------

void	Parser::parseLocation( std::string head, std::string body, std::vector<ServerConfig> &vector ) {
	StrVector shead;
	ServerConfig	&server = vector[vector.size() - 1];
	LocationConfig	location(server);

	SUtils::split(shead, head, " \t");
	std::string uri = shead[1];
	size_t pos = uri.find_first_of("{");
	if (pos != std::string::npos) {
		uri = uri.substr(0, pos);
	}
	if (uri.empty())
		throw std::logic_error("Needed one uri for location");
	location.setUri(uri);
	getLocConfig(location, location.getAD(), body);
	server.addLocationConfig(location);
	return ;
}

void Parser::getLocConfig( LocationConfig &location, StrVector allowed_directives, std::string &content ) {
	while (content.length() > 0)
	{
		try
		{
			parseLocLine(location, allowed_directives, content);
		} catch ( std::exception &e ) {
			throw std::logic_error(e.what());
		}
	}
}

void	Parser::parseLocLine( LocationConfig &vector, StrVector allowed_directives, std::string &content) {
	std::string	head;
	std::string	body;
	std::string	name;
	int			type;

	if ( (type = SUtils::get_pair(content, head, body) ) != NOT_SEPARATOR )
	{
		name = head.substr(0, head.find_first_of(ISSPACE));
		checkValidDirective(name, allowed_directives);
		checkValidSeparator(name, type);
		parseDirectiveLoc(head, vector, allowed_directives);
	}
	else
		throw std::logic_error( "Unxpected \"}\" or end of file" );
};

void	Parser::parseSimpleLoc( std::string head, LocationConfig &server, StrVector allowed_directives ) {
	std::string name = head.substr(0, head.find_first_of(ISSPACE));
	ParseContent::checkDuplicate<LocationConfig>(name, server);
	int idx = SUtils::easyfind< StrVector >(allowed_directives.begin(), allowed_directives.end(), name);
	(Parser::_parseSimpleLA[idx])(head, server);
}

void	Parser::parseDirectiveLoc( std::string head, LocationConfig &location, StrVector allowed_directives ) {
	int idx;
	std::string name;

	name = head.substr(0, head.find_first_of(ISSPACE));
	if ((idx = SUtils::easyfind< StrVector >(_simpleDirectives.begin(), _simpleDirectives.end(), name)) >= 0)
		parseSimpleLoc(head, location, allowed_directives);
}

std::string	Parser::getContent(void) {
	return (this->_content);
}