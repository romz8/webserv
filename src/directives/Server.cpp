/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 10:48:47 by jsebasti          #+#    #+#             */
/*   Updated: 2024/04/26 12:05:34 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server( void ) {
	this->directives = new Directives();
	this->directives->def_serv_name = "webserv";
	return ;
}

Server::~Server( void ) {
	if (this->directives != NULL)
		delete this->directives;
	this->directives = NULL;
	return ;
}

Server::Server( const Server & src ) {
	*this = src;
}

Server	&Server::operator=( const Server & src ) {
	if (this != &src)
	{
		this->directives = new Directives(*src.directives, 1);
	}
	return (*this);
}

void	Server::parse_server_line( std::string &s_line ) {
	std::string line = SUtils::treat_comments(s_line);
	StrVector sline;

	sline = SUtils::split(sline, line, " \t");
	size_t len = sline.size();
	if (len != 2)
		throw std::logic_error("Unexpected amount of arguments. Expected \"server {\"");
}

void	Server::parse( std::string &content ) {
	StrVector line;
	StrVector lines;
	
	lines = SUtils::split(lines, content, "\n");
	int len = lines.size();
	for (int i = 1; i < len; i++)
	{
		std::string treated_line = SUtils::treat_comments(lines[i]);
		line = SUtils::split(line, treated_line, " \t");
		if (!line[0].compare("{") || !line[0].compare("}"))
			continue ;
		int type = ParseContent::getServAllowDirectives(line[0]);
		i = ParseDirectives::parseServerDirectives( this->directives, line, type, lines, i);
		line.clear();
		treated_line.clear();
	}
}

const UintStrMap::mapped_type		&Server::getErrorPage( unsigned int error_code ) const {
	return (this->directives->getErrorPage(error_code));
}

const Location						*Server::getLocation( unsigned int num ) const {
	return (this->directives->getLocation(num));
}

int									Server::getStrongCoincidence( string name ) const {
	if (this->directives->dirSet["server_name"] == false)
		return (-1);
	for (size_t i = 0; i < this->directives->server_names.size(); i++)
	{
		if (!name.compare(this->directives->server_names[i]))
			return (i);
	}
	return (-1);
}