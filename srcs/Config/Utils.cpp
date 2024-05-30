/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 14:29:06 by jsebasti          #+#    #+#             */
/*   Updated: 2024/05/30 02:46:11 by jsebasti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Utils.hpp>
#include <iostream>

#include <ParseContent.hpp>

using namespace std;
using namespace SUtils;

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
		if (pos == string::npos)
			return (NOT_SEPARATOR);
		if (content[ pos ] == ';')
		{
			split_pair(content, head, body);
			content.erase(0, pos + 1);
			content = trim_left(content);
			return (SEMICOLON);
		}
		bracet = checkBracets(content);
		if (bracet == string::npos)
			throw logic_error("Expected }");
		head = trim( content.substr(0, pos + 1) );
		body = trim( content.substr(pos + 1, bracet - pos - 1) );
		content.erase(0, bracet + 1);
		content = trim_left(content);
		return (BRACET);
	}

	size_t	checkBracets( string content ) {
		int			level;
		size_t		bracet;
		size_t		ret;
		string	copy;

		level = 0;
		ret = 0;
		copy = content;
		do
		{
			bracet = content.find_first_of( "{}" );
			if ( bracet == string::npos \
					|| ( content[ bracet ] == '}' && level == 0 ) )
				return ( string::npos );
			level += content[ bracet ] == '{' ? 1 : -1;
			content.erase( 0, bracet + 1 );
			ret += bracet + 1;
		} while ( level > 0 );
		ret--;
		return ( ret );
	}
}

namespace std {
	int	ft_stoi( string num ) {
		int ret = 0;
	
		if (num.find_first_not_of(ISNUM) != string::npos)
			throw logic_error("Cannot do the conversion");
		
		if (num.length() > 11 && num.compare("4294967295") > 0)
			throw logic_error("The number is greater than UINT_MAX");
		
		for (size_t i = 0; i < num.length(); i++) {
			ret = (ret * 10) + (num[i]- 48);
		}
		return ret;
	}
}

void	checkValidIp( string ip ) {
	StrVector	masks;
	
	if (checkIpSyntax(ip) == false)
		throw logic_error("Invalid syntax in IP");
	split(masks, ip, ".");
	if (masks.size() > 4)
		throw logic_error("Invalid IP");
	for ( size_t i = 0; i < masks.size(); i++ )
	{
		if ( ft_stoi(masks[i]) > 255 || ft_stoi(masks[i]) < 0 )
			throw logic_error("Invalid IP");
	}
}

bool	checkIpSyntax( string ip ) {
	size_t	pos;

	pos = ip.find_first_not_of( IPVALID );
	if ( pos != string::npos || ip[ 0 ] == '.' )
		return ( false );
	pos = 0;
	while ( pos != string::npos )
	{
		pos = ip.find_first_of( "." );
		if ( pos != string::npos )
		{
			if ( ip[ pos + 1 ] == '\0' || ip[ pos + 1 ] == '.' )
				return ( false );
			ip.erase( 0, pos + 1 );
		}
	}
	return (true);
}

void	split_pair( const string str, string &head, string &body ) {
	size_t		pos;
	string	aux;

	body = "";
	aux = trim( str );
	pos = aux.find_first_of( ";" );
	if ( pos != string::npos )
	{
		head = aux.substr( 0, pos );
		body = aux.substr( pos, string::npos );
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

int	compareNumbersAsStrings( const string _num1, const string _num2 ) {
	string num1( deleteExtraZeros( _num1 ) );
	string num2( deleteExtraZeros( _num2 ) );
	if (_num1.length() > _num2.length())
		return (1);
	if (_num1.length() < _num2.length())
		return (-1);
	return (_num1.compare(_num2));
}
string	deleteExtraZeros( const string number ) {
	string res(number);
	size_t idx = res.find_first_not_of("0");
	if (idx != string::npos)
		res.erase(0, idx);
	else
		res = "0";
	return (res);
}