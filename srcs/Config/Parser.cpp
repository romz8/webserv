/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 10:36:45 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/28 13:33:01 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Parser.hpp>
#include <ServerConfig.hpp>
#include <ParseContent.hpp>
#include <LocationConfig.hpp>

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

int	Parser::checkValidDirective( std::string name, StrVector allowed_directives ) {
	if (SUtils::easyfind< StrVector >(_directives.begin(), _directives.end(), name) == -1)
		throw std::logic_error(name + " is not a valid directive");
	else if (SUtils::easyfind< StrVector >(allowed_directives.begin(), allowed_directives.end(), name) == -1)
		throw std::logic_error(name + " is not valid for this block");
	
	if (SUtils::easyfind< StrVector >(_simpleDirectives.begin(), _simpleDirectives.end(), name) >= 0)
		return (SIMPLE);
	return (SUtils::easyfind< StrVector >(_complexDirectives.begin(), _complexDirectives.end(), name));
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

std::string	Parser::getContent(void) {
	return (this->_content);
}