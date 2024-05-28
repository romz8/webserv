/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.ipp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 13:18:27 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/28 13:39:10 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PARSER_IPP__
# define __PARSER_IPP__

# include <Parser.hpp>
# include <ServerConfig.hpp>

template <typename T>
void	Parser::parseLine( std::vector<T> &vector, StrVector allowed_directives, std::string &content) {
	std::string	head;
	std::string	body;
	std::string	name;
	int			type;
	int			dirtype;

	if ( (type = SUtils::get_pair(content, head, body) ) != NOT_SEPARATOR )
	{
		name = head.substr(0, head.find_first_of(ISSPACE));
		dirtype = checkValidDirective(name, allowed_directives);
		checkValidSeparator(name, type);
		parseDirective(head, body, vector);
	}
	else
		throw std::logic_error( "Unxpected \"}\" or end of file" );
};

template <typename T>
void	Parser::parseDirective( std::string head, std::string body, std::vector<T> &vector ) {
	int idx;
	std::string name;

	name = head.substr(0, head.find_first_of(ISSPACE));
	if ((idx = SUtils::easyfind< StrVector >(_simpleDirectives.begin(), _simpleDirectives.end(), name)) >= 0)
		parseSimple(head, vector[vector.size() - 1]);
	else
		parseComplex(head, body, vector);
}

template <typename T>
void Parser::getConfig( std::vector<T> &vector, StrVector allowed_directives, std::string &content ) {
	while (content.length() > 0)
	{
		try
		{
			parseLine<T>(vector, allowed_directives, content);
		} catch ( std::exception &e ) {
			throw std::logic_error(e.what());
		}
	}
}

template <typename T>
void	Parser::parseSimple( std::string head, T &server ) {
	std::cout << head << " in server " << &server << std::endl;
}

template <typename T>
void	Parser::parseComplex( std::string head, std::string body, std::vector<T> &vector ) {
	std::string	name;
	
	name = head.substr(0, head.find_first_of(ISSPACE));
	if (!name.compare("server"))
		parseServer( head, body, vector);
	else
		parseLocation( head, body, vector);
}

template <typename T>
void	Parser::parseServer( std::string head, std::string body, std::vector<T> &vector ) {
	ServerConfig server;
	vector.push_back(server);
	std::cout << &vector[vector.size() - 1] << std::endl;
	getConfig(vector, server.getAD(), body);
}

template <typename T>
void	Parser::parseLocation( std::string head, std::string body, std::vector<T> &vector ) {
	StrVector shead;
	LocationConfig	location;
	ServerConfig	&server = vector[vector.size() - 1];

	SUtils::split(shead, head, " \t");
	std::string uri = shead[1];
	size_t pos = uri.find_first_of("{");
	if (pos != std::string::npos) {
		uri = uri.substr(0, pos);
	}
	if (uri.empty())
		throw std::logic_error("Needed one uri for location");
	location.setUri(uri);
	server.addLocationConfig(location);
	std::vector<LocationConfig> &locations = server.getLocationConfEdit();
	getConfig<LocationConfig>(locations, location.getAD(), body);
}

#endif
