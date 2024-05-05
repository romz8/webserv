/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 18:42:24 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/06 00:49:37 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

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
	this->init_sock();
}