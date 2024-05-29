/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 14:29:06 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/29 15:44:15 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Utils.hpp>
#include <iostream>

#include <ParseContent.hpp>

using namespace std;

namespace SUtils {

	void		set_allowed_directives( StrVector &allowed_methods, int flag ) {
		if (flag == SERVER)
		{
			for (int i = 0; i < N_SERVER_DIRECTIVES; i++) {
				allowed_methods.push_back(ParseContent::server_directives[i]);
			}
		}
		else
		{
			for (int i = 0; i < N_LOCATION_DIRECTIVES; i++) {
				allowed_methods.push_back(ParseContent::location_directives[i]);
			}
		}
	}

	string		&treat_comments( string & str ) {
		size_t pos = str.find_first_of("#");
	
		if ( pos != string::npos)
			str = str.substr(0, pos);
		return (str);
	}

	StrVector &	split( StrVector & v, string strArr, string delimiter ) {
		string	temp;
		size_t		starting;
		size_t		ending;
		starting = 0;
		ending = strArr.find_first_of( delimiter );
		while ( ending != string::npos )
		{
			temp = strArr.substr( starting, ending - starting );
			if ( temp.length() > 0 )
				v.push_back( temp );
			starting = ending + 1;
			ending = strArr.find_first_of( delimiter, starting );
		}
		temp = strArr.substr( starting, ending - starting );
		if ( temp.length() > 0 )
			v.push_back( temp );
		return ( v );
	}

	string	trim( string str ) {
		return (trim_right( trim_left( str ) ) );
	}

	int		get_pair( string &content, string &head, string &body ) {
		size_t	pos;
		size_t	bracet;

		content = trim_left( content );
		pos = content.find_first_of("{;");
		if (pos == std::string::npos)
			return (NOT_SEPARATOR);
		if (content[ pos ] == ';')
		{
			split_pair(content, head, body);
			content.erase(0, pos + 1);
			content = trim_left(content);
			return (SEMICOLON);
		}
		bracet = SUtils::checkBracets(content);
		if (bracet == std::string::npos)
			throw std::logic_error("Expected }");
		head = SUtils::trim( content.substr(0, pos + 1) );
		body = SUtils::trim( content.substr(pos + 1, bracet - pos - 1) );
		content.erase(0, bracet + 1);
		content = trim_left(content);
		return (BRACET);
	}

	size_t	checkBracets( string content ) {
		int			level;
		size_t		bracet;
		size_t		ret;
		std::string	copy;

		level = 0;
		ret = 0;
		copy = content;
		do
		{
			bracet = content.find_first_of( "{}" );
			if ( bracet == std::string::npos \
					|| ( content[ bracet ] == '}' && level == 0 ) )
				return ( std::string::npos );
			level += content[ bracet ] == '{' ? 1 : -1;
			content.erase( 0, bracet + 1 );
			ret += bracet + 1;
		} while ( level > 0 );
		ret--;
		return ( ret );
	}
}

void	split_pair( const string str, string &head, string &body ) {
	size_t		pos;
	std::string	aux;

	body = "";
	aux = SUtils::trim( str );
	pos = aux.find_first_of( ";" );
	if ( pos != std::string::npos )
	{
		head = aux.substr( 0, pos );
		body = aux.substr( pos, std::string::npos );
		body = trim_left( body );
	}
	else
		head = aux;
}

string trim_left( string str ) {
	size_t i = 0;

	while (i < str.length() && isspace(str[i]))
		i++;
	return (str.substr(i, str.length()));
}

string trim_right( string str ) {
	int	i;

	i = str.length() - 1;
	while ( i >= 0 && isspace( str[ i ] ) )
		i--;
	return ( str.substr( 0, i + 1 ) );
}