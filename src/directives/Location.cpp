/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 21:33:38 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 15:11:55 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location( void ) : uri("/") {
    return ;
}

Location::~Location( void ) {
    if (this->directives != NULL)
		delete this->directives;
	this->directives = NULL;
    return ;
}

Location::Location( const Location & src ) {
	*this = src;
}

Location	&Location::operator=( const Location & src ) {
	if (this != &src)
	{
		this->directives = new Directives(*src.directives);
		this->uri = src.getUri();
		this->isDir = src.isDir;
	}
	return (*this);
}

const std::string   &Location::getUri( void ) const {
    return (this->uri);
}

void                Location::setUri( const std::string & _uri ) {
    this->uri = _uri;
}

void				Location::parse_location_line( const std::string &line ) {
	StrVector sline;

	sline = SUtils::split(sline, line, " \t\n");
	size_t len = sline.size();
	if (len != 3)
		throw std::logic_error("Unexpected amount of arguments. Expected \"location /uri/ {\"");
	this->uri = sline[1];
	if (this->uri[this->uri.length() - 1] == '/')
		this->isDir = true;
	else
		this->isDir = false;
}

void    Location::parse( Directives *d, const std::string & content ) {
    StrVector line;
	StrVector lines;
    this->directives = new Directives(*d);

	lines = SUtils::split(lines, content, "\n");
	int len = lines.size();
	for (int i = 1; i < len; i++)
	{
		std::string treated_line = SUtils::treat_comments(lines[i]);
		line = SUtils::split(line, treated_line, " \t");
		if (!line[0].compare("{") || !line[0].compare("}"))
			continue ;
		int type = ParseContent::getLocAllowDirectives(line[0]);
		ParseDirectives::parseLocationDirectives( this->directives, line, type );
		line.clear();
        treated_line.clear();
	}
	if (!this->directives->alias.empty())
		this->uri = this->directives->alias;
}

const UintStrMap::mapped_type			&Location::getErrorPage( unsigned int error_code ) const {
	return (this->directives->getErrorPage(error_code));
}